#include "webserver.h"
#include <QTcpSocket>
#include "webrouter.h"
#include "utils/globals.h"
#include "sql/sqldatabase.h"
#include <QThreadPool>

namespace novastory
{

WebServer::WebServer(QObject* parent)
	: QTcpServer(parent)
{
	setObjectName("WebServer");

	qDebug() << "----------------------------------------\n";
	qDebug() << "	Novastory Server r" GIT_REVISION "\n";
	qDebug() << "-----------------------------------------\n";

	VERIFY(listen(QHostAddress::Any, 8008));
	qDebug() << "Web server started at " << serverAddress() << ":" << serverPort();
	SqlDatabase::Instance(); // first call
	QThreadPool::globalInstance()->setMaxThreadCount(5); // Maximum of working threads
	QThreadPool::globalInstance()->setExpiryTimeout(30000);
}


WebServer::~WebServer()
{
	close();
	qDebug() << "Closed webserver. Waiting while workers done.";
	// ~QThreadPool()
}

void WebServer::incomingConnection(qintptr socketDescriptor)
{
	QThreadPool::globalInstance()->start(new WebProcess(socketDescriptor));
}


}