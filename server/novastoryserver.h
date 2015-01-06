#ifndef NOVASTORYSERVER_H
#define NOVASTORYSERVER_H

#include "webserver/webserver.h"

namespace novastory
{

class NovastoryServer : public WebServer
{
public:
	NovastoryServer(QObject* parent = nullptr, quint16 initializationPort = 8008);
	virtual ~NovastoryServer();

	static NovastoryServer& Instance(quint16 initializationPort = 8008)
	{
		if (!WebServer::_self)
		{
			WebServer::_self = new NovastoryServer(nullptr, initializationPort);
		}

		return *((NovastoryServer*)WebServer::_self);
	}
};

}

#endif
