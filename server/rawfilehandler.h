#ifndef RAWFILEHANDLER_H
#define RAWFILEHANDLER_H

#include "webserver/datahandler.h"
class QTcpSocket;

namespace novastory
{

class RawFileHandler : public DataHandler
{
public:
	RawFileHandler(QTcpSocket* bindedSocket);
	virtual ~RawFileHandler();
	bool handle(const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QString& get = QString(),
				const QHash<QString, QString>& header = QHash<QString, QString>(), const QHash<QString, QString>& cookies = QHash<QString, QString>()) override;
private:
	QTcpSocket* socket;
};

}

#endif // RAWFILEHANDLER_H
