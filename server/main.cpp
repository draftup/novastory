#include <QCoreApplication>
#include "novastoryserver.h"
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
	NovastoryServer::Instance();
	int status = a.exec();
	NovastoryServer::deleteInstance();
	return status;
}
