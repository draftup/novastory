#include "apihandler.h"
#include <QDebug>
#include <QTcpSocket>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "webserver/webserver.h"
#include "logic/captcha.h"
#include "logic/user.h"
#include "logic/texteditor.h"
#include "logic/avatar.h"
#include "logic/userpic.h"
#include "logic/textrevisioncontainer.h"

namespace novastory
{

ApiHandler::ApiHandler(QTcpSocket* bindedSocket) :
	socket(bindedSocket)
{

}

ApiHandler::~ApiHandler()
{

}

bool ApiHandler::handle(const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& /* get = "" */,
						const QHash<QString, QString>& header /*= QHash<QString, QString>()*/, const QHash<QString, QString>& cookies /* = QHash<QString, QString>() */)
{
	if (path.left(4) != "/api")
	{
		return false;
	}
	qDebug() << "API protocol handled";

	QStringList hookList = path.split("/");
	if (hookList.size() != 3 && hookList.size() != 4)
	{
		return false;
	}
	if (hookList.size() == 4 && hookList[2] != "validate" && hookList[2] != "resetpassword")
	{
		return false;
	}

	QString hook = hookList[2];

	QString stoken = cookies["stoken"].isNull() ? post["stoken"] : cookies["stoken"];
	int userid = cookies["userid"].isNull() ? post["userid"].toInt() : cookies["userid"].toInt();

	QByteArray json;

	if (hook == "register")
	{
		Captcha captcha;
		captcha.setEmail(post["email"]);
		captcha.setPassword(post["password"]);
		captcha.setRemoteIP(socket->peerAddress());
		captcha.setChallenge(post["challenge"]);
		captcha.setResponse(post["response"]);
		captcha.addVerifyNotify();
		json = captcha.jsonString().toUtf8();
	}
	else if (hook == "login")
	{
		User user;
		user.login(post["email"], post["password"]);
		json = user.jsonString().toUtf8();
	}
	else if (hook == "forgotpassword")
	{
		User user;
		user.forgotPasswordMessageSend(post["email"]);
		json = user.jsonString().toUtf8();
	}
	else if (hook == "resetpassword")
	{
		User user;
		user.confirmPasswordReset(hookList[3]);
		json = user.jsonString().toUtf8();
	}
	else if (hook == "validate")
	{
		User* newUser = User::verifyUser(hookList[3]);
		if (newUser)
		{
			json = newUser->jsonString().toUtf8();
		}
		delete newUser;
	}
	else if (hook == "editorupdate")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextEditor editor;
		editor.setUser(user);
		editor.setText(post["text"]);
		editor.update();
		json = editor.jsonString().toUtf8();
	}
	else if (hook == "editortext")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextEditor editor;
		editor.setUser(user);
		editor.sync();
		json = editor.jsonString().toUtf8();
	}
	else if (hook == "revisionslist")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextRevisionContainer container;
		container.setUser(user);
		container.sync(post["folder"].toInt());
		json = container.json(true).toUtf8();
	}
	else if (hook == "revisions")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextRevisionContainer container;
		container.setUser(user);
		container.sync();
		json = container.json(false).toUtf8();
	}
	else if (hook == "revision")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextRevisionContainer container;
		container.setUser(user);
		TextRevision rev = container.revision(post["revision"].toInt());
		if (rev.isValid())
		{
			QJsonDocument doc;
			doc.setObject(rev.json(false));
			json = doc.toJson();
		}
	}
	else if (hook == "revisioninsert")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextRevisionContainer container;
		container.setUser(user);
		container.setText(post["text"]);
		container.setMark(post["mark"]);
		container.insert(post["parent"].toInt());
		json = container.jsonString().toUtf8();
	}
	else if (hook == "revisionupdate")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextRevisionContainer container;
		container.setUser(user);
		container.setText(post["text"]);
		container.setMark(post["mark"]);
		int rev = post["revision"].toInt();
		container.update(rev);
		json = container.jsonString().toUtf8();
	}
	else if (hook == "revisionupdatemark")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextRevisionContainer container;
		container.setUser(user);
		container.setMark(post["mark"]);
		int rev = post["revision"].toInt();
		container.updateMark(rev);
		json = container.jsonString().toUtf8();
	}
	else if (hook == "revisionremove")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextRevisionContainer container;
		container.setUser(user);
		container.removeRevision(post["revision"].toInt());
		json = container.jsonString().toUtf8();
	}
	else if (hook == "release")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextRevisionContainer container;
		container.setUser(user);
		VERIFY(container.release(post["revision"].toInt()));
		json = container.jsonString().toUtf8();
	}
	else if (hook == "unrelease")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextRevisionContainer container;
		container.setUser(user);
		VERIFY(container.unrelease(post["revision"].toInt()));
		json = container.jsonString().toUtf8();
	}
	else if (hook == "revdirs")
	{
		User user;
		user.loginByToken(userid, stoken);
		TextRevisionContainer container;
		container.setUser(user);
		json = container.treeFolders().toUtf8();
	}
	else if (hook == "subscribe")
	{
		User user;
		user.loginByToken(userid, stoken);
		User target;
		target.setUserID(post["targetid"].toInt());
		user.subscribe(target);
		json = user.jsonString().toUtf8();
	}
	else if (hook == "unsubscribe")
	{
		User user;
		user.loginByToken(userid, stoken);
		User target;
		target.setUserID(post["targetid"].toInt());
		user.unsubscribe(target);
		json = user.jsonString().toUtf8();
	}
	else if (hook == "subscriptions")
	{
		User user;
		user.setUserID(post["userid"].toInt());
		QList<User> subscriptions = user.subscriptions();
		QJsonDocument doc;
		QJsonArray arr;
		for (User& id : subscriptions)
		{
			arr.push_back(id.userid());
		}
		doc.setArray(arr);
		json = doc.toJson();
	}
	else if (hook == "subscribed")
	{
		User user;
		user.setUserID(post["userid"].toInt());
		QList<User> subscriptions = user.subscribed();
		QJsonDocument doc;
		QJsonArray arr;
		for (User& id : subscriptions)
		{
			arr.push_back(id.userid());
		}
		doc.setArray(arr);
		json = doc.toJson();
	}
	else if (hook == "issubscribed")
	{
		User user;
		user.setUserID(userid);
		User tuser;
		tuser.setUserID(post["targetid"].toInt());
		QJsonDocument doc;
		QJsonObject obj;
		obj.insert("issubscribed", QJsonValue(user.isSubscribed(tuser)));
		doc.setObject(obj);
		json = doc.toJson();
	}
	else if (hook == "updateavatar")
	{
		Avatar avatar;
		avatar.setAvatar(WebDataContainer(post["avatar"]));
		User user;
		user.loginByToken(userid, stoken);
		avatar.setUser(user);
		if (avatar.update())
		{
			WebServer::Instance().cache().remove(("/avatar/" + QString::number(avatar.userid())).toStdString());
			WebServer::Instance().cache().remove(("/avatar/" + avatar.email()).toStdString());
		}
		json = avatar.jsonString().toUtf8();
	}
	else if (hook == "updateuserpic")
	{
		UserPic userpic;
		userpic.setUserPic(WebDataContainer(post["userpic"]));
		User user;
		user.loginByToken(userid, stoken);
		userpic.setUser(user);
		if (userpic.update())
		{
			WebServer::Instance().cache().remove(("/userpic/" + QString::number(userpic.userid())).toStdString());
			WebServer::Instance().cache().remove(("/userpic/" + userpic.email()).toStdString());
		}
		json = userpic.jsonString().toUtf8();
	}
	else if (hook == "updateprofile")
	{
		User user;
		user.loginByToken(userid, stoken);
		user.setFirstName(post["firstname"]);
		user.setLastName(post["lastname"]);
		user.setNickName(post["nickname"]);
		user.setProfileId(post["profileid"]);
		user.update();
		json = user.jsonString().toUtf8();
	}
	else if (hook == "myprofile")
	{
		User user;
		user.loginByToken(userid, stoken);
		user.appendProfileJson();
		json = user.jsonString().toUtf8();
	}
	else if (hook == "version")
	{
		// Default
		QJsonDocument version;
		QJsonObject versionObject;
		versionObject.insert("engine", QJsonValue(QString("novastory")));
		versionObject.insert("version", QJsonValue(QString(GIT_VERSION)));
		versionObject.insert("describe", QJsonValue(QString(GIT_DESCRIBE)));
		versionObject.insert("revision", QJsonValue(QString(GIT_REVISION).toInt()));
		version.setObject(versionObject);
		json = version.toJson();
	}

	socket->write(htmlHeaderGen("application/json", json.size()));
	socket->write(json);

	return true;
}

}
