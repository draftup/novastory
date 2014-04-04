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

	static WebServer& Instance()
	{
		static WebServer theSingleInstance;
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
