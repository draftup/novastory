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
	virtual ~WebRouter();
	QString path() const;
	QString postData() const;
	QString getData() const;
	void sendHtml();
	void parse() override;

	void parsePost();
	void parseGet();
	void parseCookie();
	void parseLanguage();

	QString coockie(const QString& name);

private:
	void parseParams(const QString& data, QHash<QString, QString>* var);
	QHash<QString, QString> postVariables;
	QHash<QString, QString> getVariables;
	QHash<QString, QString> cookieVariables;
};

}

#endif
