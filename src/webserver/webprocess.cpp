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

	eventLoop->exit();
}
}