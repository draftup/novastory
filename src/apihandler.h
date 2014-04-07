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
	bool handle(const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QString& get = QString(),
				const QHash<QString, QString>& cookies = QHash<QString, QString>()) override;
private:
	QTcpSocket* socket;
};

}

#endif // APIHANDLER_H
