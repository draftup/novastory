#include "webserver.h"
#include <QTcpSocket>
#include "webrouter.h"
#include "globals.h"
#include "sql/sqldatabase.h"
#include "sql/dbpatcher.h"
#include <QThreadPool>
#include <QDir>
#include "webserver_config.h"
#include <QMimeDatabase>
#include "logger.h"
#include <QCoreApplication>
#include <QMutexLocker>
#include "datahandler.h"

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif
#include <QFile>

namespace novastory
{

WebServer* WebServer::_self = nullptr;

WebServer::WebServer(QObject* parent, quint16 initializationPort /*=8008*/, const QString& pid_file /* = "default_app.pid" */, const QString& db_file /* = "default_db.h" */)
	: QTcpServer(parent), webCache(CACHE_SIZE)
	, m_pid_name(pid_file), m_db_file(db_file),
	  m_public_dir("public")
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
			QFile file(m_db_file);
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

	FILE* fp = fopen((QCoreApplication::instance()->applicationDirPath() + "/" + m_pid_name).toLatin1().data(), "w");
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
	QDir dataDirectory = QDir(DATA_DIRECTORY + QString("/" + m_public_dir));
	if (dataDirectory.exists())
	{
		publicDirectory = dataDirectory.absolutePath();
	}
#ifdef REMOTE_TESTING
	else if (QDir(QDir::currentPath() + "/.." + "/" + m_public_dir).exists())
	{
		publicDirectory = QDir::currentPath() + "/.." + "/" + m_public_dir;
	}
#endif
	else if (QDir(QDir::currentPath() + "/../.." + "/" + m_public_dir).exists())
	{
		publicDirectory = QDir::currentPath() + "/../.." + "/" + m_public_dir;
	}
	else if (QDir(QDir::currentPath() + "/" + m_public_dir).exists())
	{
		publicDirectory = QDir::currentPath() + "/" + m_public_dir;
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

QString WebServer::pidFile() const
{
	return m_pid_name;
}

void WebServer::setPidFile(const QString& pid)
{
	m_pid_name = pid;
}

QString WebServer::dbFile() const
{
	return m_db_file;
}

void WebServer::setDBFile(const QString& db)
{
	m_db_file = db;
}

void WebServer::addDefaultLanguage(const QString& language)
{
	QMutexLocker locker(&default_language_mutex);
	default_language.insert(QThread::currentThread(), language);
}

void WebServer::removeDefaultLanguage()
{
	QMutexLocker locker(&default_language_mutex);
	default_language.remove(QThread::currentThread());
}


QString WebServer::defaultLanguage()
{
	QMutexLocker locker(&default_language_mutex);
	return default_language.value(QThread::currentThread());
}

QList<QString> WebServer::languageList() const
{
	return translators.keys();
}

}