#include <QCoreApplication>
#include "webserver/webserver.h"
#include "sql/dbpatcher.h"
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

	// Now we can start our application
	WebServer::Instance();

	return a.exec();
}
