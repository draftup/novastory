#include "webrouter.h"
#include <QTcpSocket>
#include "utils.h"

namespace novastory
{

WebRouter::WebRouter(QTcpSocket *bindedSocket)
    : bindedSocket(bindedSocket)
{

}

void WebRouter::parse()
{
	VERIFY(bindedSocket != nullptr);

	qDebug() << bindedSocket->readAll();
}

}