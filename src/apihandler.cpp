#include "apihandler.h"
#include <QDebug>
#include <QTcpSocket>

#include <QJsonDocument>
#include <QJsonObject>

#include "logic/captcha.h"
#include "logic/user.h"
#include "logic/texteditor.h"

namespace novastory
{

ApiHandler::ApiHandler(QTcpSocket* bindedSocket) :
	socket(bindedSocket)
{

}

ApiHandler::~ApiHandler()
{

}

bool ApiHandler::handle(const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& /* get = "" */)
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
	if (hookList.size() == 4 && hookList[2] != "validate")
	{
		return false;
	}

	QString hook = hookList[2];

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
	else if (hook == "validate")
	{
		User* newUser = User::verifyUser(hookList[3]);
		json = newUser->jsonString().toUtf8();
		delete newUser;
	}
	else if (hook == "editorupdate")
	{
		User user;
		if (user.loginByToken(post["email"], post["token"]))
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

	socket->write(
		("HTTP/1.1 200 OK\n"
		"Server: novastory\n"
		"Content-Type: application/json\n"
		"Content-Length: " + QString::number(json.size()) + "\n\n").toLatin1()
	);
	socket->write(json);

	return true;
}

}
