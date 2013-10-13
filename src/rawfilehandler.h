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
	QByteArray data(const QString& path = "") override;
	QMimeType mimeType() const override;
};

}

#endif // RAWFILEHANDLER_H
