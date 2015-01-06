#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QTcpServer>
#include "webprocess.h"
#include "utils/bytecache.hpp"

namespace novastory
{

class DataHandler;
class WebRouter;

class WebServer : public QTcpServer
{
	Q_OBJECT
	friend class WebRouter;
public:
	static WebServer& Instance(quint16 initializationPort = 8008)
	{
		if (!_self)
		{
			_self = new WebServer(nullptr, initializationPort);
		}

		return *_self;
	}

	static bool deleteInstance()
	{
		if (_self)
		{
			delete _self;
			_self = 0;
			return true;
		}
		return false;
	}

	void setDirectory(const QString& path);
	QString directory() const;
	void resetDirectory();
	ByteCache& cache();
protected:
	WebServer(QObject* parent = nullptr, quint16 initializationPort = 8008);
	virtual ~WebServer();

	static WebServer* _self;

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
