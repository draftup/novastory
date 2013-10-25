#ifndef APIHANDLER_H
#define APIHANDLER_H

#include "datahandler.h"
class QTcpSocket;

namespace novastory
{

class ApiHandler : public DataHandler
{
public:
	ApiHandler(QTcpSocket* bindedSocket);
	virtual ~ApiHandler();
	bool handle(const QString& path) override;
private:
	QTcpSocket* socket;
};

}

#endif // APIHANDLER_H
