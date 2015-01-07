#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QString>
#include <QHash>
#include <QString>

class QTcpSocket;

namespace novastory
{

class DataHandler
{
public:
	virtual bool handle(
		QTcpSocket* socket,
		const QString& type,
		const QString& path,
		const QHash<QString, QString>& post = QHash<QString, QString>(),
		const QString& get = QString(),
		const QHash<QString, QString>& header = QHash<QString, QString>(),
		const QHash<QString, QString>& cookies = QHash<QString, QString>()
	) = 0;
};

}

#endif
