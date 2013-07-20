#ifndef WEBROUTER_H
#define WEBROUTER_H

namespace novastory
{

class QTcpSocket;

class WebRouter
{

public:
    WebRouter(QTcpSocket* bindedSocket);

private:
    QTcpSocket* bindedSocket;

};

}

#endif
