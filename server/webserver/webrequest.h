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
	const QHash<QString, QString>& header() const {
		return parsedValues;
	};
protected:
	QTcpSocket* bindedSocket;
	QHash<QString, QString> parsedValues;
};

}

#endif
