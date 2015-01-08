#ifndef ERRORHANDLER_H
#define RAWFILEHANDLER_H

#include "../datahandler.h"
class QTcpSocket;

namespace novastory
{

class ErrorHandler : public DataHandler
{
public:
	bool handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QString& get = QString(),
				const QHash<QString, QString>& header = QHash<QString, QString>(), const QHash<QString, QString>& cookies = QHash<QString, QString>()) override;
	virtual bool handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post = QHash<QString, QString>(), const QString& get = QString(),
						const QHash<QString, QString>& header = QHash<QString, QString>(), const QHash<QString, QString>& cookies = QHash<QString, QString>(), int error_type = 0);
};

}

#endif // RAWFILEHANDLER_H
