#ifndef WEBROUTER_H
#define WEBROUTER_H

#include "webrequest.h"
#include "htmlhandler.h"
class QTcpSocket;
#include <QString>

namespace novastory
{

class WebRouter : public WebRequest
{
public:
	WebRouter(QTcpSocket *bindedSocket);
	QString path() const;
	void appendHandler(HtmlHandler* handler);
	void removeHandler(HtmlHandler* handler);
	void sendHtml();
private:
	QList<HtmlHandler*> handlers;
};

}

#endif
