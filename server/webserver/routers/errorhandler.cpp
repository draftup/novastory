#include "errorhandler.h"

namespace novastory
{

bool ErrorHandler::handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */,
							const QHash<QString, QString>& header /*= QHash<QString, QString>()*/, const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/)
{
	return false;
}

bool ErrorHandler::handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post /*= QHash<QString, QString>()*/, const QString& get /*= QString()*/, const QHash<QString, QString>& header /*= QHash<QString, QString>()*/, const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/, int error_type /*= 0*/)
{
	return false;
}


}
