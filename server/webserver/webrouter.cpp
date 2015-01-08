#include "webrouter.h"
#include "webserver.h"
#include "globals.h"
#include "routers/errorhandler.h"
#include <QTextStream>
#include <QTcpSocket>
#include <QUrl>

namespace novastory
{
WebRouter::WebRouter(QTcpSocket* bindedSocket) : WebRequest(bindedSocket)
{
	/*

	*/
}

QString WebRouter::path() const
{
	return parsedValues["path"];
}

void WebRouter::sendHtml()
{
	bool isHandeled = false;
	ErrorHandler* errorHandler = nullptr;
	for (QSharedPointer<DataHandler> handler : WebServer::Instance().handlers)
	{
		isHandeled |= handler->handle(bindedSocket, parsedValues["type"], path(), postVariables, QString(), parsedValues, cookieVariables);
		if (errorHandler == nullptr)
		{
			errorHandler = dynamic_cast<ErrorHandler*>(handler.data());
		}
	}
	if (!isHandeled)
	{
		qDebug() << "404 Error, page not founded";
		if (errorHandler)
		{
			VERIFY(errorHandler->handle(bindedSocket, parsedValues["type"], path(), postVariables, QString(), parsedValues, cookieVariables, 404));
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