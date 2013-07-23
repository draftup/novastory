#ifndef WEBREQUEST_H
#define WEBREQUEST_H

class QTcpSocket;
#include <QHash>

namespace novastory
{

class WebRequest
{

public:
    WebRequest(QTcpSocket* bindedSocket);
	virtual void parse();
	virtual bool isParsed() const;
protected:
    QTcpSocket* bindedSocket;
	QHash<QString, QString> parsedValues;
};

}

#endif
