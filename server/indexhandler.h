#ifndef INDEXHANDLER_H
#define INDEXHANDLER_H

#include "webserver/datahandler.h"
class QTcpSocket;

namespace novastory
{

class IndexHandler : public DataHandler
{
public:
	bool handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QString& get = QString(),
				const QHash<QString, QString>& header = QHash<QString, QString>(), const QHash<QString, QString>& cookies = QHash<QString, QString>()) override;
};

}

#endif
