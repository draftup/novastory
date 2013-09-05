#include "webserver.h"
#include <QTcpSocket>
#include "utils.h"
#include "webrouter.h"

namespace novastory
{

WebServer::WebServer(QObject *parent)
    : QTcpServer(parent)
{
    listen(QHostAddress::Any, 8008);
	VERIFY(connect(this, SIGNAL(newConnection()), this, SLOT(someNewConnection())));
}


WebServer::~WebServer()
{

}

void WebServer::someNewConnection()
{
    QTcpSocket* clientSocket = nextPendingConnection();
	qDebug() << clientSocket;
	VERIFY( connect(clientSocket, SIGNAL(readyRead()), this, SLOT(showHtmlPage())) );
}

void WebServer::showHtmlPage()
{
	QTcpSocket* clientSocket = (QTcpSocket*)sender();

	WebRouter urlRouter(clientSocket);
	urlRouter.parse();
	qDebug() << "path = " << urlRouter.path();
	urlRouter.sendHtml();

	clientSocket->close();
}

}