#include "webprocess.h"
#include "webrouter.h"
#include <QTcpSocket>
#include "globals.h"
#include "webserver_config.h"
#include <QThreadPool>
#include <QTimer>
#include "sql/sqldatabase.h"

namespace novastory
{

std::atomic<int> WebProcess::processCounter(0);

WebProcess::WebProcess(int socket_descriptor) : socketDescriptor(socket_descriptor)
{
	setObjectName(QString("WebProcess ") + socket_descriptor);
	setAutoDelete(true);
	qDebug() << "THREAD WorkProcess started (now active instances: " << ++WebProcess::processCounter << ")";
}

WebProcess::~WebProcess()
{
	qDebug() << "THREAD WorkProcess closed (now active instances: " << --WebProcess::processCounter << ")";
}

void WebProcess::run()
{
	// All classes must be initizalized in WebProcess Thread
	socket.reset(new QTcpSocket);
	eventLoop.reset(new QEventLoop);
	timeout.reset(new QTimer);
	VERIFY(socket->setSocketDescriptor(socketDescriptor));
	timeout->setSingleShot(true);
	qDebug() << "Socket" << socketDescriptor << "openned";

	// Connecting all socket slots
	VERIFY(connect(socket.data(), SIGNAL(readyRead()), this, SLOT(showHtmlPage()), Qt::DirectConnection));
	VERIFY(connect(socket.data(), SIGNAL(aboutToClose()), this, SLOT(onSocketClosed()), Qt::DirectConnection));
	VERIFY(connect(socket.data(), SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()), Qt::DirectConnection));
	VERIFY(connect(socket.data(), SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWriten(qint64)), Qt::DirectConnection));
	VERIFY(connect(timeout.data(), SIGNAL(timeout()), this, SLOT(closedByInterval()), Qt::DirectConnection));

	// Start event loop for this process worker (until disconnect of socket)
	timeout->start(WORKER_TIME_LIMIT * 1000); // 60 second session limit
	eventLoop->exec();

	// Closing all db related to this thread
	SqlDatabase::close(QThread::currentThreadId());
}

void WebProcess::closeSocket()
{
	while (socket->bytesToWrite() > 0 && socket->state() != QAbstractSocket::UnconnectedState)
	{
		if (!socket->waitForBytesWritten(WORKER_DISCONNECT_TIMEOUT))
			break;
	}

	socket->close();
}

void WebProcess::showHtmlPage()
{
	WebRouter urlRouter(socket.data());

	urlRouter.parse();
	qDebug() << "path = " << urlRouter.path();
	urlRouter.sendHtml();

	// Закрываем соединением, если запрос не поддерживает Keep-Alive
	if (!urlRouter.header()["Connection"].toLower().contains("keep-alive")) {
		closeSocket();
	}
}

void WebProcess::onSocketDisconnected()
{
	eventLoop->exit();
}

void WebProcess::onSocketClosed()
{
	qDebug() << "Socket" << socket->socketDescriptor() << "closed";
}

void WebProcess::onBytesWriten(qint64 bytes)
{
	qDebug() << "Socket bytes written: " << bytes;
}

void WebProcess::closedByInterval()
{
	qDebug() << "Closed socket by interval";
	closeSocket();
}

}