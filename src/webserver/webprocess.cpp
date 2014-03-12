#include "webprocess.h"
#include "webrouter.h"
#include <QTcpSocket>
#include "utils/globals.h"
#include <QThreadPool>

namespace novastory
{

int WebProcess::processCounter = 0;

WebProcess::WebProcess(int socket_descriptor) : socketDescriptor(socket_descriptor)
{
	setAutoDelete(true);
	qDebug() << "THREAD WorkProcess started (now active instances: " << ++WebProcess::processCounter << ")";
}

WebProcess::~WebProcess()
{
	qDebug() << "THREAD WorkProcess closed (now active instances: " << --WebProcess::processCounter << ")";
}

void WebProcess::run()
{
	socket.reset(new QTcpSocket);
	VERIFY(socket->setSocketDescriptor(socketDescriptor));
	VERIFY(connect(socket.data(), SIGNAL(readyRead()), this, SLOT(showHtmlPage()), Qt::DirectConnection));
	eventLoop.reset(new QEventLoop);
	VERIFY(connect(socket.data(), SIGNAL(aboutToClose()), this, SLOT(onSocketClosed()), Qt::DirectConnection));
	VERIFY(connect(socket.data(), SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWriten(qint64)), Qt::DirectConnection));
	eventLoop->exec();
}


void WebProcess::showHtmlPage()
{
	WebRouter urlRouter(socket.data());

	urlRouter.parse();
	qDebug() << "path = " << urlRouter.path();
	urlRouter.sendHtml();

	socket->waitForBytesWritten();
	socket->close();
}

void WebProcess::onSocketClosed()
{
	eventLoop->exit();
}

void WebProcess::onBytesWriten(qint64 bytes)
{
	qDebug() << "Socket bytes written: " << bytes;
}

}