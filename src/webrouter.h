#ifndef WEBROUTER_H
#define WEBROUTER_H

class QTcpSocket;

namespace novastory
{

class WebRouter
{

public:
    WebRouter(QTcpSocket* bindedSocket);
	void parse();
private:
    QTcpSocket* bindedSocket;

};

}

#endif
