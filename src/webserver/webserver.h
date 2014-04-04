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
	WebServer(QObject* parent = nullptr, quint16 initializationPort = 8008);
	virtual ~WebServer();

	static WebServer& Instance(quint16 initializationPort = 8008)
	{
		static WebServer theSingleInstance(nullptr, initializationPort);
		return theSingleInstance;
	}

	void setDirectory(const QString& path);
	QString directory() const;
	void resetDirectory();
protected:
	void incomingConnection(qintptr socketDescriptor) override;

private:
	QString publicDirectory;
};

}

#endif
