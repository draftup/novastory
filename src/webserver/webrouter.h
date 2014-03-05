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
private:
	QList<DataHandler*> handlers;
	QHash<QString, QString> postVariables;
};

}

#endif
