#include "apihandler.h"
#include <QDebug>
#include <QTcpSocket>

#include <QJsonDocument>
#include <QJsonObject>

#include "webserver/webserver.h"
#include "logic/captcha.h"
#include "logic/user.h"
#include "logic/texteditor.h"
#include "logic/avatar.h"

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
		json = newUser->jsonString().toUtf8();
		delete newUser;
	}
	else if (hook == "editorupdate")
	{
		User user;
		if (user.loginByToken(post["email"], stoken))
		{
			TextEditor editor;
			editor.setUserID(user.userid());
			editor.setText(post["text"]);
			json = editor.jsonString().toUtf8();
		}
		else
		{
			json = user.jsonString().toUtf8();
		}
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
