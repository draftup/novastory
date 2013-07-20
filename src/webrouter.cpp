#include "webrouter.h"
#include <QTcpSocket>

namespace novastory
{

WebRouter::WebRouter(QTcpSocket *bindedSocket)
    : bindedSocket(bindedSocket)
{

}

}