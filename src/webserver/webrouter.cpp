#include "webrouter.h"
#include "rawfilehandler.h"
#include "apihandler.h"
#include "avatarshandler.h"
#include "profilehandler.h"
#include "utils/globals.h"
#include "webserver/templator.h"
#include <QTextStream>
#include <QTcpSocket>
#include <QUrl>

namespace novastory
{
WebRouter::WebRouter(QTcpSocket* bindedSocket) : WebRequest(bindedSocket), socket(bindedSocket)
{
	appendHandler(new RawFileHandler(bindedSocket));
	appendHandler(new ApiHandler(bindedSocket));
	appendHandler(new AvatarsHandler(bindedSocket));
	appendHandler(new ProfileHandler(bindedSocket));
}

QString WebRouter::path() const
{
	return parsedValues["path"];
}

void WebRouter::removeHandler(DataHandler* handler)
{
	QMutableListIterator< QSharedPointer<DataHandler> > i(handlers);
	while (i.hasNext())
	{
		if (i.next().data() == handler)
		{
			i.remove();
		}
	}
}

void WebRouter::appendHandler(DataHandler* handler)
{
	handlers.append(QSharedPointer<DataHandler>(handler));
}

void WebRouter::sendHtml()
{
	bool isHandeled = false;
	for (QSharedPointer<DataHandler> handler : handlers)
	{
		isHandeled |= handler->handle(parsedValues["type"], path(), postVariables, QString(), parsedValues, cookieVariables);
	}
	if (!isHandeled)
	{
		qDebug() << "404 Error, page not founded";
		if (!coockie("userid").isNull())
		{
			User user;
			if (user.loginByToken(coockie("userid").toInt(), coockie("stoken")))
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
	}
}

const QString& WebRouter::postData() const
{
	return parsedValues["POST"];
}

void WebRouter::parsePost()
{
	QString data = postData();
	if (data.isEmpty())
	{
		return;
	}

	for (QString pair : data.split("&"))
	{
		QStringList keyValuePair = pair.split("=");

		if (keyValuePair.size() != 2)
		{
			continue;
		}

		postVariables.insert(keyValuePair[0], QUrl::fromPercentEncoding(keyValuePair[1].replace(QChar('+'), "%20").toUtf8()));
	}
}

void WebRouter::parse()
{
	WebRequest::parse();
	parsePost();
	parseCookie();
}

void WebRouter::parseCookie()
{
	QString data = parsedValues["Cookie"];
	if (data.isEmpty())
	{
		return;
	}

	for (QString pair : data.split("; "))
	{
		QStringList keyValuePair = pair.split("=");

		if (keyValuePair.size() != 2)
		{
			continue;
		}

		auto element = cookieVariables.insert(keyValuePair[0], keyValuePair[1].trimmed());
		qDebug() << "Parsed cookie " << element.key() << "=" << element.value();
	}
}

QString WebRouter::coockie(const QString& name)
{
	return cookieVariables[name];
}

}