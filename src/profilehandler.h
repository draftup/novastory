#ifndef PROFILEHANDLER_H
#define PROFILEHANDLER_H

#include "datahandler.h"
class QTcpSocket;

namespace novastory
{

class ProfileHandler : public DataHandler
{
public:
	ProfileHandler(QTcpSocket* bindedSocket);
	virtual ~ProfileHandler();
	bool handle(const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QString& get = QString(),
				const QHash<QString, QString>& header = QHash<QString, QString>(), const QHash<QString, QString>& cookies = QHash<QString, QString>()) override;
private:
	QTcpSocket* socket;
};

}

#endif
