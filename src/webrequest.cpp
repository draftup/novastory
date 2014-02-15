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
	QStringList dataArray = data.split("\n");

	qDebug() << "WEB REQUEST: " << data;

	if(dataArray.size() == 0)
	{
		qDebug() << "Browser agent don't recive any info";
		return;
	}

	QRegExp getRx("(GET|POST) (.*) HTTP/([0-9.]+)");
	int pos = 0;
	if ((pos = getRx.indexIn(dataArray[0])) != -1)
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
	int coINT = 0;
	QString postData;
	for(int i = 1; i < dataArray.size(); ++i)
	{
		int posParam = 0;
		if ((posParam = paramRx.indexIn(dataArray[i])) != -1)
		{
			QString param = paramRx.cap(1);
			QString value = paramRx.cap(2);
			parsedValues[param] = value;
		}
		else if(dataArray[i] == "\r")
		{
			coINT = i;
		}

		if(parsedValues["type"] == "POST" && coINT > 0 && coINT != i)
		{
			postData += (i-1 == coINT) ? dataArray[i] : '\n' + dataArray[i];
		}
	}

	if(parsedValues["type"] == "POST")
	{
		parsedValues["POST"] = postData;

		qDebug() << "POST data: " << postData;
	}
	

	if(postData.size() > 0 && !parsedValues["Content-Length"].isEmpty() && postData.size() != parsedValues["Content-Length"].toInt())
	{
		qDebug() << "Wrong post size";
	}
}

bool WebRequest::isParsed() const
{
	return parsedValues.size() > 0;
}

}