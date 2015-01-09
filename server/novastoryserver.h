#ifndef NOVASTORYSERVER_H
#define NOVASTORYSERVER_H

#include "webserver/webserver.h"

namespace novastory
{

class NovastoryServer : public WebServer
{
public:
	NovastoryServer(QObject* parent = nullptr, quint16 initializationPort = 8008, const QString& pid_file = "default_app.pid", const QString& db_file = "default_db.h");
	virtual ~NovastoryServer();

	static NovastoryServer& Instance(quint16 initializationPort = 8008, const QString& pid_file = "default_app.pid", const QString& db_file = "default_db.h")
	{
		if (!WebServer::_self)
		{
			WebServer::_self = new NovastoryServer(nullptr, initializationPort, pid_file, db_file);
		}

		return *((NovastoryServer*)WebServer::_self);
	}
};

}

#endif
