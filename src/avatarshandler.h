#ifndef AVATARSHANDLER_H
#define AVATARSHANDLER_H

#include "datahandler.h"
#include "logic/avatar.h"
class QTcpSocket;

namespace novastory
{

class AvatarsHandler : public DataHandler
{
public:
	AvatarsHandler(QTcpSocket* bindedSocket);
	virtual ~AvatarsHandler();
	bool handle(const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QString& get = QString(),
				const QHash<QString, QString>& cookies = QHash<QString, QString>()) override;

	void showAvatar(const Avatar& avatar);
private:
	QTcpSocket* socket;
};

}

#endif
