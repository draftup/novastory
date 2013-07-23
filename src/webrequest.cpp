#include "webrequest.h"
#include <QTcpSocket>
#include "utils.h"
#include <QRegExp>

namespace novastory
{

WebRequest::WebRequest(QTcpSocket *bindedSocket)
    : bindedSocket(bindedSocket)
{

}

void WebRequest::parse()
{
	QString data = bindedSocket->readAll();
	QRegExp getRx("GET (.*) HTTP/([0-9.]+)");
	int pos = 0;
	if((pos = getRx.indexIn(data)) != -1)
	{
		parsedValues["path"] = getRx.cap(1);
		parsedValues["html_varsion"] = getRx.cap(2);
		pos = getRx.matchedLength();
	}
}

bool WebRequest::isParsed() const
{
	return parsedValues.size() > 0;
}

}