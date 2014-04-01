#include "webrouter.h"
#include "rawfilehandler.h"
#include "apihandler.h"
#include <QTextStream>
#include <QTcpSocket>
#include <QUrl>

namespace novastory
{
WebRouter::WebRouter(QTcpSocket* bindedSocket) : WebRequest(bindedSocket), socket(bindedSocket)
{
	appendHandler(new RawFileHandler(bindedSocket));
	appendHandler(new ApiHandler(bindedSocket));
}

QString WebRouter::path() const
{
	return parsedValues["path"];
}

void WebRouter::removeHandler(DataHandler* handler)
{
	handlers.removeAll(handler);
}

void WebRouter::appendHandler(DataHandler* handler)
{
	handlers.append(handler);
}

void WebRouter::sendHtml()
{
	bool isHandeled = false;
	for (DataHandler* handler : handlers)
	{
		isHandeled |= handler->handle(parsedValues["type"], path(), postVariables);
	}
	if(!isHandeled)
	{
		qDebug() << "404 Error, page not founded";
		socket->write(
			"HTTP/1.1 404 Not Found\n"
			"Server: Novastory\n"
			"Connection: close\n"
			"Content-Length: 16\n"
			"Content-Type: text/html\n\n"
			"404 Not founded."
		);
	}
}

QString WebRouter::postData() const
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

		postVariables.insert(keyValuePair[0], QUrl::fromPercentEncoding(keyValuePair[1].toUtf8()));
	}
}

void WebRouter::parse()
{
	WebRequest::parse();
	parsePost();
}

}