#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QTcpServer>
#include "webprocess.h"
#include "bytecache.hpp"
#include <QSharedPointer>
#include <QHash>
#include "translatorhelper.h"

class QTranslator;

namespace novastory
{

class DataHandler;
class WebRouter;

class WebServer : public QTcpServer
{
	Q_OBJECT
	friend class WebRouter;
	struct WebTranslator
	{
		QSharedPointer<QTranslator> translator;
		QSharedPointer<TranslatorHelper> helper;
	};
public:
	static WebServer& Instance(quint16 initializationPort = 8008, const QString& pid_file = "default_app.pid", const QString& db_file = "default_db.h")
	{
		if (!_self)
		{
			_self = new WebServer(nullptr, initializationPort, pid_file, db_file);
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

	QHash<QString, WebTranslator>& webTranslators()
	{
		return translators;
	}

	QString pidFile() const;
	void setPidFile(const QString& pid);

	QString dbFile() const;
	void setDBFile(const QString& pid);
protected:
	WebServer(QObject* parent = nullptr, quint16 initializationPort = 8008, const QString& pid_file = "default_app.pid", const QString& db_file = "default_db.h");
	virtual ~WebServer();

	static WebServer* _self;

	void incomingConnection(qintptr socketDescriptor) override;

	void appendHandler(DataHandler* handler);
	void removeHandler(DataHandler* handler);
private:
	QString publicDirectory;
	ByteCache webCache;
	QList< QSharedPointer<DataHandler> > handlers;
	QString m_pid_name;
	QString m_db_file;
	QString m_public_dir;
	QHash<QString, WebTranslator> translators;
};

}

#endif
