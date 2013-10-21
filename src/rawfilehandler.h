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
	bool handle(const QString& path) override;
	void setDirectory(const QString& path);
	QString directory() const;
private:
	QTcpSocket *socket;
	QString workingDirectory;
};

}

#endif // RAWFILEHANDLER_H
