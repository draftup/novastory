#include <QCoreApplication>
#include "webserver.h"
#include "logger.h"

using namespace novastory;

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	Logger::Instance().setWriteToLogFile(true); // Log all to file output
	WebServer w;

	return a.exec();
}
