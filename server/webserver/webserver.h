#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QTcpServer>
#include "webprocess.h"
#include "utils/bytecache.hpp"

namespace novastory
{

class DataHandler;

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
	ByteCache& cache();
protected:
	void incomingConnection(qintptr socketDescriptor) override;

	void appendHandler(DataHandler* handler);
	void removeHandler(DataHandler* handler);
private:
	QString publicDirectory;
	ByteCache webCache;
	QList< QSharedPointer<DataHandler> > handlers;
};

}

#endif
