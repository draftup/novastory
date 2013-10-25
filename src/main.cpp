#include <QCoreApplication>
#include "webserver.h"

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	novastory::WebServer w;

	return a.exec();
}
