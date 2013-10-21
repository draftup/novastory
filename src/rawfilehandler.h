#ifndef RAWFILEHANDLER_H
#define RAWFILEHANDLER_H

#include "datahandler.h"
class QTcpSocket;

namespace novastory
{

class RawFileHandler : public DataHandler
{
public:
	RawFileHandler(QTcpSocket *bindedSocket);
	~RawFileHandler();
	void handle(const QString& path) override;
private:
	QTcpSocket *socket;
};

}

#endif // RAWFILEHANDLER_H
