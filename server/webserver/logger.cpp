#include "logger.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QThread>
#include "globals.h"
#include "webserver_config.h"

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
	errorFile(nullptr),
	errorStream(nullptr),
	htmlFile(nullptr),
	htmlStream(nullptr),
	isWriteToFile(false),
	failReports(false)
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
	delete errorStream;
	delete errorFile;
	delete htmlStream;
	delete htmlFile;
}

void Logger::initializeFileLog()
{
	if (logFile || logStream)
	{
		return;    // Already initialized file log
	}

	QString logPath;
	QString errorPath;
	QString htmlPath;
	logPath = PROJECT_NAME ".log";
	errorPath = PROJECT_NAME ".errors.log";
	htmlPath = PROJECT_NAME ".html";
	Q_ASSERT(!logFile);
	Q_ASSERT(!errorFile);
	Q_ASSERT(!htmlFile);
	logFile = new QFile(logPath);
	logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
	errorFile = new QFile(errorPath);
	errorFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
	htmlFile = new QFile(htmlPath);
	htmlFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append);
	Q_ASSERT(!logStream);
	Q_ASSERT(!errorStream);
	Q_ASSERT(!htmlStream);
	logStream = new QTextStream(logFile);
	errorStream = new QTextStream(errorFile);
	htmlStream = new QTextStream(htmlFile);

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

	log(message, type);

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
			previousMsgHandler(type, context, QDateTime::currentDateTimeUtc().toString("[hh:mm:ss] ") + msg);
#else
			previousMsgHandler(type, msg);
#endif
		}
	}
}

void Logger::log(const QString& text, QtMsgType type)
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

		if (errorStream)
		{
			switch (type)
			{
			case QtCriticalMsg:
			case QtFatalMsg:
			{
				*errorStream << record;
				errorStream->flush();

				if (failReports && logFile)
				{
					QFile* file = new QFile(QFileInfo(*logFile).absoluteFilePath());
					// In Fatal way we must wait until app sending message will finished than crash
					if (type == QtCriticalMsg)
						sendAsyncMail(REPORTS_MAIL, "Novastory failure", record, QList<QFile*>({ file }));
					else
						sendMail(REPORTS_MAIL, "Novastory failure", record, QList<QFile*>({ file }), true);
				}
			}
			default:
				break;
			}
		}

		if (htmlStream)
		{
			QString html = record;
			switch (type)
			{
			case QtDebugMsg:
				html = "<div class=\"debug\">" + html + "</div>";
				break;
			case QtWarningMsg:
				html = "<div class=\"warning\">" + html + "</div>";
				break;
			case QtCriticalMsg:
				html = "<div class=\"critical\">" + html + "</div>";
				break;
			case QtFatalMsg:
				html = "<div class=\"fatal\">" + html + "</div>";
			}

			*htmlStream << html;
			htmlStream->flush();
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

void Logger::setFailReports(bool enable)
{
	failReports = enable;
}

QByteArray Logger::html() const
{
	QFile html(PROJECT_NAME ".html");
	if (!html.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCritical() << "Html log file not founded";
		return QByteArray();
	}

	return html.readAll();
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
