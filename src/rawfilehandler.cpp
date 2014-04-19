#include "rawfilehandler.h"
#include "utils/globals.h"
#include "webserver/webserver.h"
#include <QDebug>
#include <QFile>
#include <QTcpSocket>

#include <QMimeDatabase>
#include <QMimeType>

namespace novastory
{

RawFileHandler::RawFileHandler(QTcpSocket* bindedSocket) :
	socket(bindedSocket)
{
}

RawFileHandler::~RawFileHandler()
{

}

bool RawFileHandler::handle(const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */,
							const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/)
{
	const QString workingDirectory = WebServer::Instance().directory();
	QString filePath = workingDirectory + path;

	if (path == "/")
	{
		filePath = workingDirectory + "/index.html";
	}

	QFile existFile(filePath);
	if (existFile.exists())
	{
		// First, looking in cache
		QByteArray inCacheData = WebServer::Instance().cache().get(existFile.symLinkTarget().toStdString());
		if (!inCacheData.isNull())
		{

		}

		if (existFile.open(QIODevice::ReadOnly))
		{
			qDebug() << "Raw file handler: " << filePath;

			QByteArray data = existFile.readAll();

			QMimeDatabase db;
			QMimeType mime = db.mimeTypeForFileNameAndData(existFile.fileName(), data);

			qDebug() << "Raw file type is: " << mime.name();

			socket->write(htmlHeaderGen(mime.name(), data.size()));
			socket->write(data);

			return true;
		}
	}

	return false;
}


}
