#ifndef WEBROUTER_H
#define WEBROUTER_H

#include "webrequest.h"
class QTcpSocket;
#include <QString>

namespace novastory
{

class WebRouter : public WebRequest
{
public:
	WebRouter(QTcpSocket *bindedSocket);
	QString path() const;
};

}

#endif
