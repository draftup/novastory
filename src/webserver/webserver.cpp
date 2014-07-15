#include "webserver.h"
#include <QTcpSocket>
#include "webrouter.h"
#include "utils/globals.h"
#include "sql/sqldatabase.h"
#include "sql/dbpatcher.h"
#include <QThreadPool>
#include <QDir>
#include "config.h"
#include <QMimeDatabase>

// pregenerated db
#include "database/novastory_db.h"

namespace novastory
{

WebServer::WebServer(QObject* parent, quint16 initializationPort /*=8008*/)
	: QTcpServer(parent), webCache(CACHE_SIZE)
{
	setObjectName("WebServer");

	resetDirectory();

	// Initialize mime database
	QMimeDatabase().mimeTypeForFile("index.html");

	VERIFY(listen(QHostAddress::Any, initializationPort));
	qDebug() << "Web server started at " << serverAddress() << ":" << serverPort();
	QThreadPool::globalInstance()->setMaxThreadCount(WORKERS_NUMBER); // Maximum of working threads
	qDebug() << "Maximum workers number: " << WORKERS_NUMBER;
	QThreadPool::globalInstance()->setExpiryTimeout(WORKERS_MAX_TIME * 1000);

	// Update db to last version
	DBPatcher patcher;
	patcher.setDatabaseStructure(DB_TABLE_STRUCT());
	patcher.patch();
}


WebServer::~WebServer()
{
	close();
	qDebug() << "Closed webserver. Waiting while workers done.";
	// ~QThreadPool()
}

void WebServer::incomingConnection(qintptr socketDescriptor)
{
	QThreadPool::globalInstance()->start(new WebProcess(socketDescriptor));
}

void WebServer::setDirectory(const QString& path)
{
	QDir targetDirectory(path);
	if (targetDirectory.exists())
	{
		publicDirectory = targetDirectory.absolutePath();
	}
}

void WebServer::resetDirectory()
{
	QDir dataDirectory = QDir(DATA_DIRECTORY + QString("/public"));
	if (dataDirectory.exists())
	{
		publicDirectory = dataDirectory.absolutePath();
	}
	else if (QDir(QDir::currentPath() + "/../.." + "/public").exists())
	{
		publicDirectory = QDir::currentPath() + "/../.." + "/public";
	}
	else if (QDir(QDir::currentPath() + "/public").exists())
	{
		publicDirectory = QDir::currentPath() + "/public";
	}
	else
	{
		Q_ASSERT(false && "No public directory founded");
	}
}

QString WebServer::directory() const
{
	return publicDirectory;
}

ByteCache& WebServer::cache()
{
	return webCache;
}


}