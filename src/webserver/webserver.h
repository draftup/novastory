#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QTcpServer>
#include "webprocess.h"

namespace novastory
{

class WebServer : public QTcpServer
{
	Q_OBJECT

public:
	WebServer(QObject* parent = 0);
	virtual ~WebServer();

protected:
	void incomingConnection(qintptr socketDescriptor) override;

};

}

#endif
