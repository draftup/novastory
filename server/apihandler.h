#ifndef APIHANDLER_H
#define APIHANDLER_H

#include "webserver/datahandler.h"
class QTcpSocket;

namespace novastory
{

class ApiHandler : public DataHandler
{
public:
	bool handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QString& get = QString(),
				const QHash<QString, QString>& header = QHash<QString, QString>(), const QHash<QString, QString>& cookies = QHash<QString, QString>()) override;
};

}

#endif // APIHANDLER_H
