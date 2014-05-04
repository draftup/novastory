#include "webrequest.h"
#include <QTcpSocket>
#include "utils/globals.h"
#include <QRegExp>
#include "config.h"

namespace novastory
{

WebRequest::WebRequest(QTcpSocket* bindedSocket)
	: bindedSocket(bindedSocket)
{

}

void WebRequest::parse()
{
	QString data = bindedSocket->read(CONTENT_LIMIT_BYTES);

	QStringList dataArray = data.split("\n\r\n");
	QStringList headerArray = dataArray[0].split("\n");
	QString body = (dataArray.size() == 2) ? dataArray[1] : QString();

	qDebug() << "Requst Header: " << dataArray[0];

	dataArray.clear();

	if (headerArray.size() == 0)
	{
		qWarning() << "Browser agent don't recive any header info";
		return;
	}

	QRegExp getRx("(GET|POST|HEAD) (.*) HTTP/([0-9.]+)");
	if (getRx.indexIn(headerArray[0]) != -1)
	{
		parsedValues["type"] = getRx.cap(1);
		parsedValues["path"] = getRx.cap(2);
		parsedValues["html_version"] = getRx.cap(3);

		qDebug() << "Web request type is: " << parsedValues["type"];
		qDebug() << "Web request path is: " << parsedValues["path"];
	}
	else
	{
		qWarning() << "Doesn't looks like browser request, aborting";
		return;
	}

	// Parsing requst lines
	QRegExp paramRx("(.*): (.*)");
	for (int i = 1; i < headerArray.size(); ++i)
	{
		if (paramRx.indexIn(headerArray[i]) != -1)
		{
			QString param = paramRx.cap(1);
			QString value = paramRx.cap(2).trimmed();
			parsedValues[param] = value;
		}
	}

	if (parsedValues["Content-Length"].toInt() > CONTENT_LIMIT_BYTES)
	{
		qDebug() << "Too big request, aborting";
		return;
	}

	if (body.size() > 0 && !parsedValues["Content-Length"].isEmpty() && body.size() != parsedValues["Content-Length"].toInt())
	{
		int length = parsedValues["Content-Length"].toInt();
		while (body.size() != length)
		{
			if (!bindedSocket->waitForReadyRead())
			{
				qDebug() << "Connection timeout";
				return;
			}
			body.append(bindedSocket->readAll());
			if (body.size() > length)
			{
				qCritical() << "Wrong request size. Very wrong.";
				return;
			}
		}
	}

	if (parsedValues["type"] == "POST")
	{
		parsedValues["POST"] = body;
		qDebug() << "POST data: " << ((body.size() > 1024) ? body.left(1024) : body);
	}

}

bool WebRequest::isParsed() const
{
	return parsedValues.size() > 0;
}

}