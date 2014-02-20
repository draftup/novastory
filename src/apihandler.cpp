#include "apihandler.h"
#include <QDebug>
#include <QTcpSocket>

#include <QJsonDocument>
#include <QJsonObject>

#include "logic/captcha.h"
#include "logic/user.h"

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
	if(hookList.size() != 3)
	{
		return false;
	}

	QString hook = hookList[2];

	if (hook == "register")
	{
		Captcha captcha;
		captcha.setEmail(post["email"]);
		captcha.setPassword(post["password"]);
		captcha.setRemoteIP(socket->peerAddress());
		captcha.setChallenge(post["challenge"]);
		captcha.setResponse(post["response"]);
		captcha.addVerifyNotify();
		socket->write(captcha.jsonString().toUtf8());
	}
	else if (hook == "login")
	{
		User user;
		user.login(post["email"], post["password"]);
		socket->write(user.jsonString().toUtf8());
	}
	else
	{
		// Default
		QJsonDocument version;
		QJsonObject versionObject;
		versionObject.insert("version", QJsonValue(QString(GIT_REVISION).toInt()));
		version.setObject(versionObject);
		socket->write(version.toJson());
	}



	return true;
}

}
