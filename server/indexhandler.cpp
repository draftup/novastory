#include "indexhandler.h"
#include "utils/globals.h"
#include <QDebug>
#include <QTcpSocket>
#include <QFile>
#include "webserver/templator.h"
#include "webserver/webserver.h"

namespace novastory
{

IndexHandler::IndexHandler(QTcpSocket* bindedSocket) :
	socket(bindedSocket)
{
}

IndexHandler::~IndexHandler()
{

}

bool IndexHandler::handle(const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */,
						  const QHash<QString, QString>& header /* = QHash<QString, QString>()*/, const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/)
{

	if (path == "/" || path == "/index.php" || path == "/index.html" || path == "/index.htm" || path == "/index")
	{
		User loginedUser;
		if (cookies.contains("userid"))
		{
			loginedUser.loginByToken(cookies["userid"].toInt(), cookies["stoken"]);
		}

		const QString workingDirectory = WebServer::Instance().directory();
		QFile templateFile(workingDirectory + "/index-page.html");
		if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qCritical() << "Template not founded";
			return true;
		}

		QString data = templateFile.readAll();

		QString indexElement = selectorId(data, "indexpage");
		QByteArray responce = Templator::generate(loginedUser, "Novastory Engine", indexElement);
		socket->write(htmlHeaderGen("text/html", responce.size()));
		socket->write(responce);

		return true;
	}

	return false;
}


}
