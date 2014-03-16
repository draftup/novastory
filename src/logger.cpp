#include "logger.h"
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QThread>

namespace novastory
{

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
static QtMessageHandler previousMsgHandler = nullptr;
void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& message);
#else
static QtMsgHandler previousMsgHandler = nullptr;
void messageOutput(QtMsgType type, const char* msg);
#endif

Logger::Logger() :
	logFile(nullptr),
	logStream(nullptr),
	isWriteToFile(false)
{

}

Logger::~Logger()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	qInstallMessageHandler(0);
#else
	qInstallMsgHandler(0);
#endif
	delete logStream;
	delete logFile;
}

void Logger::initializeFileLog()
{
	if (logFile || logStream)
	{
		return;    // Already initialized file log
	}

	QString path;
#ifdef Q_OS_WIN
	path = "novastory.log";
#else
	//path = "/var/log/novastory.log";
	path = "novastory.log";
#endif
	Q_ASSERT(!logFile);
	logFile = new QFile(path);
	logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
	Q_ASSERT(!logStream);
	logStream = new QTextStream(logFile);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	previousMsgHandler = qInstallMessageHandler(&novastory::messageOutput);
#else
	previousMsgHandler = qInstallMsgHandler(&novastory::messageOutput);
#endif
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
void Logger::messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
#else
void Logger::messageOutput(QtMsgType type, const char* msg)
#endif
{
	QString message;

	switch (type)
	{
	case QtDebugMsg:
		message = "[Debug] ";
		break;
	case QtWarningMsg:
		message = "[Warning] ";
		break;
	case QtCriticalMsg:
		message = "[Critical] ";
		break;
	case QtFatalMsg:
		message = "[Fatal] ";
	}

	message += msg;

	log(message);

	if (type == QtFatalMsg)
	{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
		qInstallMessageHandler(0);
		qt_message_output(type, context, msg);
#else
		qInstallMsgHandler(0);
		qt_message_output(type, msg);
#endif
	}
	else
	{
		if (previousMsgHandler != nullptr)
		{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			previousMsgHandler(type, context, msg);
#else
			previousMsgHandler(type, msg);
#endif
		}
	}
}

void Logger::log(const QString& text)
{
	if (isWriteToFile)
	{
		QString record = QDateTime::currentDateTimeUtc().toString("dd.MM.yy hh:mm:ss:zzz") + ": "
						 + QString::number((unsigned long long) QThread::currentThreadId()) + ' ' + text + '\n';

		QMutexLocker locker(&m_mutex);

		if (logStream)
		{
			*logStream << record;
			logStream->flush();
		}
	}
}

void Logger::setWriteToLogFile(bool writToFile)
{
	isWriteToFile = writToFile;
	if (isWriteToFile)
	{
		initializeFileLog();
	}
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
	Logger::Instance().messageOutput(type, context, message);
}
#else
void messageOutput(QtMsgType type, const char* msg)
{
	Logger::Instance().messageOutput(type, msg);
}
#endif

}
