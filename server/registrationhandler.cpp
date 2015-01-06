#include "registrationhandler.h"
#include "utils/globals.h"
#include <QDebug>
#include <QTcpSocket>
#include "logic/captcha.h"
#include "logic/user.h"
#include "webserver/templator.h"

namespace novastory
{

bool RegistrationHandler::handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */,
								 const QHash<QString, QString>& header /*= QHash<QString, QString>()*/, const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/)
{
	if (path.startsWith("/validate/"))
	{
		QString token = path.mid((int)strlen("/validate/"));
		if (token.size() != 32)
		{
			qWarning() << "Wrong token given";
			return false;
		}

		qDebug() << "Registation handler working. Now will try to activate new user";

		User* newUser = User::verifyUser(token);
		QByteArray responce;
		if (newUser)
		{
			responce = Templator::generate("Novastory Engine", "Registration successful. Welcome to Novastory!");
		}
		else
		{
			responce = Templator::generate("Novastory Engine", "Registration failed. May be token is too old? Try again or contact administator.");
		}
		delete newUser;

		socket->write(htmlHeaderGen("text/html", responce.size()));
		socket->write(responce);

		return true;
	}
	return false;
}

}
