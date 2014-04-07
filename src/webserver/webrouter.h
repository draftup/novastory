#ifndef WEBROUTER_H
#define WEBROUTER_H

#include "webrequest.h"
#include "datahandler.h"
class QTcpSocket;
#include <QString>
#include <QHash>

namespace novastory
{

class WebRouter : public WebRequest
{
public:
	WebRouter(QTcpSocket* bindedSocket);
	QString path() const;
	QString postData() const;
	void appendHandler(DataHandler* handler);
	void removeHandler(DataHandler* handler);
	void sendHtml();
	void parse() override;

	void parsePost();

	void parseCookie();
	QString coockie(const QString& name);

private:
	QTcpSocket* socket;
	QList<DataHandler*> handlers;
	QHash<QString, QString> postVariables;
	QHash<QString, QString> cookieVariables;
};

}

#endif
