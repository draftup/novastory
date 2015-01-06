#ifndef REGISTRATIONHANDLER_H
#define REGISTRATIONHANDLER_H

#include "webserver/datahandler.h"
#include "logic/avatar.h"
class QTcpSocket;

namespace novastory
{

class RegistrationHandler : public DataHandler
{
public:
	RegistrationHandler(QTcpSocket* bindedSocket);
	virtual ~RegistrationHandler();
	bool handle(const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QString& get = QString(),
				const QHash<QString, QString>& header = QHash<QString, QString>(), const QHash<QString, QString>& cookies = QHash<QString, QString>()) override;
private:
	QTcpSocket* socket;
};

}

#endif // REGISTRATIONHANDLER_H
