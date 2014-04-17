#include <QCoreApplication>
#include "webserver/webserver.h"
#include "logger.h"
#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

using namespace novastory;

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

#ifdef Q_OS_LINUX
	for (QString arg : a.arguments())
	{
		if (arg == "-d")
		{
			if (daemon(true, false))
			{
				qFatal("Cannot run in daemon mode");
			}
		}
	}

	pid_t pid = getpid();

	FILE* fp = fopen("novastory.pid", "w");
	if (!fp)
	{
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fprintf(fp, "%d\n", pid);
	fclose(fp);
#endif

	Logger::Instance().setWriteToLogFile(true); // Log all to file output
	WebServer::Instance();

	return a.exec();
}
