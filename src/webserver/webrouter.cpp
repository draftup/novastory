#include "webrouter.h"
#include "rawfilehandler.h"
#include "apihandler.h"
#include <QTextStream>
#include <QTcpSocket>
#include <QUrl>

namespace novastory
{
WebRouter::WebRouter(QTcpSocket* bindedSocket) : WebRequest(bindedSocket)
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
	//QTextStream os(bindedSocket);
	//os.setAutoDetectUnicode(true);
	//os << "HTTP/1.0 200 Ok\r\n"
	//   << "Content-Type: image/jpeg\n\n";;
	for (DataHandler* handler : handlers)
	{
		handler->handle(parsedValues["type"], path(), postVariables);
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