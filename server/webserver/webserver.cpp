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
#include "websocketslistener.h"
#ifdef Q_OS_LINUX
#include <QLoggingCategory>
#endif

#ifdef Q_OS_LINUX
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>
#endif
#include <QFile>

namespace novastory
{

#ifdef Q_OS_LINUX
void handler_sigfault(int sig)
{
	void* buffer[15];
	char** strings;
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(buffer, 15);

	// print out all the frames to stderr
	qDebug() << "Sigfault with signal:" << sig;
	strings = backtrace_symbols(buffer, size);
	if (strings == NULL)
	{
		qFatal("backtrace_symbols");
		exit(EXIT_FAILURE);
	}
	for (int j = 0; j < size; j++)
	{
		qDebug() << strings[j];
	}
	free(strings);
	qDebug() << "Application closed";
	exit(1);
}
#endif

WebServer* WebServer::_self = nullptr;

WebServer::WebServer(QObject* parent, quint16 initializationPort /*=8008*/, const QString& pid_file /* = "default_app.pid" */, const QString& db_file /* = "default_db.h" */)
	: QTcpServer(parent), webCache(CACHE_SIZE), m_maintenance(false)
	, m_pid_name(pid_file), m_db_file(db_file),
	  m_public_dir("public"), webSocketListener(nullptr)
#ifndef QT_DEBUG
	, webSecureSocketListener(nullptr)
#endif
{
	setObjectName("WebServer");

	// Fix bug with log messages
#ifdef Q_OS_LINUX
	QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
#endif
	Logger::Instance().setWriteToLogFile(true); // Log all to file output
#ifndef QT_DEBUG
	Logger::Instance().setFailReports(true);
#endif

#ifdef Q_OS_LINUX
	signal(SIGSEGV, handler_sigfault);
#endif

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
			exit(EXIT_SUCCESS);
			return;
		}
	}

	resetDirectory();

	// Initialize mime database
	QMimeDatabase().mimeTypeForFile("index.html");

	VERIFY(listen(QHostAddress::Any, initializationPort));
	qDebug() << "Web server started at " << serverAddress() << ":" << serverPort();
	QThreadPool::globalInstance()->setMaxThreadCount(WORKERS_NUMBER); // Maximum of working threads
	qDebug() << "Maximum workers number: " << WORKERS_NUMBER;
	QThreadPool::globalInstance()->setExpiryTimeout(WORKERS_MAX_TIME * 1000);

#ifndef QT_DEBUG
	webSecureSocketListener = new WebSocketsListener(8081, true);
#endif // !QT_DEBUG
	webSocketListener = new WebSocketsListener(8082, false);
}


WebServer::~WebServer()
{
	close();
	qDebug() << "Closed webserver. Waiting while workers done.";
	QThreadPool::globalInstance()->waitForDone();
	qDebug() << "Thread pool closed all connections.";
	if (webSocketListener != nullptr)
	{
		webSocketListener->deleteLater();
	}
#ifndef QT_DEBUG
	if (webSecureSocketListener != nullptr)
	{
		webSecureSocketListener->deleteLater();
	}
#endif // !QT_DEBUG
}

void WebServer::incomingConnection(qintptr socketDescriptor)
{
	if (!m_maintenance)
	{
		QThreadPool::globalInstance()->start(new WebProcess(socketDescriptor));
	}
	else
	{
		maintenanceRespond(socketDescriptor);
	}
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
	else if (QDir(SOURCE_DIRECTORY + QString("/" + m_public_dir)).exists())
	{
		publicDirectory = QDir(SOURCE_DIRECTORY + QString("/" + m_public_dir)).absolutePath();
	}
	else
	{
		Q_ASSERT(false && "No public directory founded");
		qCritical() << "No public directory founded";
	}

	qDebug() << "Set public directory path:" << publicDirectory;
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

#if defined(VSTEAMS_BUILD)
void WebServer::setPreferedGame(int gameid)
{
	QMutexLocker locker(&prefered_game_mutex);
	prefered_game.insert(QThread::currentThread(), gameid);
}

void WebServer::removePreferedGame()
{
	QMutexLocker locker(&prefered_game_mutex);
	prefered_game.remove(QThread::currentThread());
}

int WebServer::preferedGame()
{
	QMutexLocker locker(&prefered_game_mutex);
	return prefered_game.value(QThread::currentThread());
}
#endif

void WebServer::setMaintenance(bool is_maintenance)
{
	m_maintenance = is_maintenance;
}

bool WebServer::broadcastWSocketsTextMessage(const QString& message, const QString& filter /* = QString() */, const QVariant& filterValue /* = QVariant() */)
{
	QMutexLocker locker(&websokets_mutex);
	bool message1 = false, message2 = false;
	if (webSocketListener != nullptr)
	{
		message1 = webSocketListener->broadcastTextMessage(message, filter, filterValue);
	}
#ifndef QT_DEBUG
	if (webSecureSocketListener != nullptr)
	{
		message2 = webSecureSocketListener->broadcastTextMessage(message, filter, filterValue);
	}
#endif

	return message1 || message2;
}

void WebServer::maintenanceRespond(int socket_descriptor)
{
	qDebug() << "Server maintenance. Sending respond";
	QTcpSocket* socket = new QTcpSocket;
	socket->setSocketDescriptor(socket_descriptor);

	VERIFY(connect(socket, &QTcpSocket::readyRead, [socket]()
	{
		qDebug() << "Server maintenance. Sending ignore respond";
		socket->write(novastory::htmlData("Server maintenance", "text/html", "503 Service Unavailable"));
		socket->close();
	}));

	connect(socket, SIGNAL(disconnected()),
			socket, SLOT(deleteLater()));
}

}
