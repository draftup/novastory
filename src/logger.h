#ifndef LOGGER_H
#define LOGGER_H

#include <QtGlobal>
#include <QMutex>
class QFile;
class QTextStream;

namespace novastory
{

class Logger
{
public:
	explicit Logger();
	~Logger();
	static Logger& Instance()
	{
		static Logger theSingleInstance;
		return theSingleInstance;
	}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);
#else
	void messageOutput(QtMsgType type, const char* msg);
#endif

	void log(const QString& text);

	void setWriteToLogFile(bool writToFile);
private:
	void initializeFileLog();

	QFile* logFile;
	QTextStream* logStream;
	bool isWriteToFile;
	QMutex m_mutex;
};

}

#endif // LOGGER_H
