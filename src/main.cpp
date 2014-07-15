#include <QCoreApplication>
#include "webserver/webserver.h"
#include "sql/dbpatcher.h"
#include "logger.h"
#ifdef Q_OS_LINUX
#include <unistd.h>
#endif
#include <QFile>

using namespace novastory;

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	qDebug() << "----------------------------------------\n";
	qDebug() << "	Novastory Server r" GIT_REVISION "\n";
	qDebug() << "-----------------------------------------\n";

	Logger::Instance().setWriteToLogFile(true); // Log all to file output

	// Now we can start our application

	for (QString arg : a.arguments())
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
			return 0;
		}
	}

	// save pid to file
#ifdef Q_OS_LINUX
	pid_t pid = getpid();

	FILE* fp = fopen((a.applicationDirPath() + "/novastory.pid").toLatin1().data(), "w");
	if (!fp)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "%d\n", pid);
	fclose(fp);
#endif

	WebServer::Instance();

	return a.exec();
}
