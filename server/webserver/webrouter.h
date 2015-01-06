#ifndef WEBROUTER_H
#define WEBROUTER_H

#include "webrequest.h"
#include "datahandler.h"
class QTcpSocket;
#include <QString>
#include <QHash>
#include <QSharedPointer>

namespace novastory
{

class WebRouter : public WebRequest
{
public:
	WebRouter(QTcpSocket* bindedSocket);
	QString path() const;
	const QString& postData() const;
	void sendHtml();
	void parse() override;

	void parsePost();

	void parseCookie();
	QString coockie(const QString& name);

private:
	QTcpSocket* socket;
	QHash<QString, QString> postVariables;
	QHash<QString, QString> cookieVariables;
};

}

#endif
