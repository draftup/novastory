#include "novastoryerrorhandler.h"
#include "logic/user.h"
#include <QTcpSocket>
#include "webserver/templator.h"

namespace novastory
{

bool NovastoryErrorHandler::handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */,
								   const QHash<QString, QString>& header /*= QHash<QString, QString>()*/, const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/, int error_type)
{
	if (error_type > 0)
	{
		if (!cookies["userid"].isNull())
		{
			User user;
			if (user.loginByToken(cookies["userid"].toInt(), cookies["stoken"]))
			{
				QByteArray responce = Templator::generateLogined(user, "Page not founded", "<div style=\"text-align: center;\"><img src=\"/images/404.jpg\" /></div>");
				socket->write(htmlHeaderGen("text/html", responce.size(), "404 Not Found") + responce);
			}
		}
		else
		{
			QByteArray responce = Templator::generate("Page not founded", "<div style=\"text-align: center;\"><img src=\"/images/404.jpg\" /></div>");
			socket->write(htmlHeaderGen("text/html", responce.size(), "404 Not Found") + responce);
		}

		return true;
	}
	return false;
}


}
