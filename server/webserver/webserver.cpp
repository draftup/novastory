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
#include "logger.h"
#include <QCoreApplication>
#include "webserver/datahandler.h"

// pregenerated db
#include "database/novastory_db.h"

namespace novastory
{

WebServer* WebServer::_self = nullptr;

WebServer::WebServer(QObject* parent, quint16 initializationPort /*=8008*/)
	: QTcpServer(parent), webCache(CACHE_SIZE)
{
	setObjectName("WebServer");

	Logger::Instance().setWriteToLogFile(true); // Log all to file output
#ifndef QT_DEBUG
	Logger::Instance().setFailReports(true);
#endif

	for (QString arg : QCoreApplication::instance()->arguments())
	{
#ifdef Q_OS_LINUX
		if (arg == "-d")
		{
			if (daemon(true, false))
			{
				qFatal("Cannot run in daemon mode");
			}
		}
#endif
		if (arg == "-s")
		{
			QFile file("novastory_db.h");
			if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
			{
				QTextStream stream(&file);
				stream << DBPatcher::cppSerialize();
			}
			file.close();

			qDebug() << "DB struct saved to file, run server without -s now";
			QCoreApplication::instance()->quit();
			return;
		}
	}

	// save pid to file
#ifdef Q_OS_LINUX
	pid_t pid = getpid();

	FILE* fp = fopen((QCoreApplication::instance()->applicationDirPath() + "/novastory.pid").toLatin1().data(), "w");
	if (!fp)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "%d\n", pid);
	fclose(fp);
#endif

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

void WebServer::removeHandler(DataHandler* handler)
{
	QMutableListIterator< QSharedPointer<DataHandler> > i(handlers);
	while (i.hasNext())
	{
		if (i.next().data() == handler)
		{
			i.remove();
		}
	}
}

void WebServer::appendHandler(DataHandler* handler)
{
	handlers.append(QSharedPointer<DataHandler>(handler));
}

}