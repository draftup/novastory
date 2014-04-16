#include "webrequest.h"
#include <QTcpSocket>
#include "utils/globals.h"
#include <QRegExp>

namespace novastory
{

WebRequest::WebRequest(QTcpSocket* bindedSocket)
	: bindedSocket(bindedSocket)
{

}

void WebRequest::parse()
{
	QString data = bindedSocket->readAll();

	QStringList dataArray = data.split("\n\r\n");
	QStringList headerArray = dataArray[0].split("\n");
	QString body = (dataArray.size() == 2) ? dataArray[1] : QString();
	dataArray.clear();

	qDebug() << "WEB REQUEST: " << data;

	if (headerArray.size() == 0)
	{
		qDebug() << "Browser agent don't recive any info";
		return;
	}

	QRegExp getRx("(GET|POST) (.*) HTTP/([0-9.]+)");
	int pos = 0;
	if ((pos = getRx.indexIn(headerArray[0])) != -1)
	{
		parsedValues["type"] = getRx.cap(1);
		parsedValues["path"] = getRx.cap(2);
		parsedValues["html_version"] = getRx.cap(3);

		qDebug() << "Web request type is: " << parsedValues["type"];
		qDebug() << "Web request path is: " << parsedValues["path"];

		pos = getRx.matchedLength();
	}
	else
	{
		qDebug() << "Doesn't looks like browser request, aborting";
		return;
	}

	// Parsing requst lines
	QRegExp paramRx("(.*): (.*)");
	for (int i = 1; i < headerArray.size(); ++i)
	{
		int posParam = 0;
		if ((posParam = paramRx.indexIn(headerArray[i])) != -1)
		{
			QString param = paramRx.cap(1);
			QString value = paramRx.cap(2);
			parsedValues[param] = value;
		}
	}

	if (body.size() > 0 && !parsedValues["Content-Length"].isEmpty() && body.size() != parsedValues["Content-Length"].toInt())
	{
		while (body.size() != parsedValues["Content-Length"].toInt())
		{
			bindedSocket->waitForReadyRead();
			body.append(bindedSocket->readAll());
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