#ifndef AVATARSHANDLER_H
#define AVATARSHANDLER_H

#include "webserver/datahandler.h"
#include "logic/avatar.h"
class QTcpSocket;

namespace novastory
{

class AvatarsHandler : public DataHandler
{
public:
	bool handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QString& get = QString(),
				const QHash<QString, QString>& header = QHash<QString, QString>(), const QHash<QString, QString>& cookies = QHash<QString, QString>()) override;
};

}

#endif
