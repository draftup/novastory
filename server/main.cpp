#include <QCoreApplication>
#include "novastoryserver.h"

using namespace novastory;

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	qDebug() << "----------------------------------------\n";
	qDebug() << "	Novastory Server r" GIT_REVISION "\n";
	qDebug() << "-----------------------------------------\n";

	// Now we can start our application
	NovastoryServer::Instance(8008, "novastory.pid", "novastory_db.h");
	int status = a.exec();
	NovastoryServer::deleteInstance();
	return status;
}
