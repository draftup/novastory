#ifndef WEBROUTER_H
#define WEBROUTER_H

#include "webrequest.h"
#include "datahandler.h"
class QTcpSocket;
#include <QString>

namespace novastory
{

class WebRouter : public WebRequest
{
public:
	WebRouter(QTcpSocket *bindedSocket);
	QString path() const;
	void appendHandler(DataHandler* handler);
	void removeHandler(DataHandler* handler);
	void sendHtml();
private:
	QList<DataHandler*> handlers;
};

}

#endif
