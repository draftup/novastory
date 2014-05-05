#include "rawfilehandler.h"
#include "utils/globals.h"
#include "webserver/webserver.h"
#include <QDebug>
#include <QFile>
#include <QTcpSocket>

#include <QMimeDatabase>
#include <QMimeType>
#include <QFileInfo>

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
							const QHash<QString, QString>& header /*= QHash<QString, QString>()*/, const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/)
{
	const QString workingDirectory = WebServer::Instance().directory();
	QString filePath = workingDirectory + path;

	QFile existFile(filePath);
	if (existFile.exists())
	{
		// First, looking in cache
		try
		{
			WebDataContainer inCacheData = WebServer::Instance().cache().get(filePath.toStdString());
			qDebug() << "Readed from cache " << path << "(Current cache size:" << WebServer::Instance().cache().currentSize() << ")";

			QString controlEtag = header["If-None-Match"];
			QString eTag = inCacheData.eTag();

			if (controlEtag.size() > 0 && controlEtag == eTag)
			{
				qDebug() << "Requested cache data is good, sending only header";
				socket->write(htmlHeaderGen(QString(), -1, "304 Not Modified"));
			}
			else
			{
				socket->write(htmlHeaderGen(inCacheData));
				socket->write(inCacheData);
			}

			return true;
		}
		catch (std::range_error&)
		{
			if (existFile.open(QIODevice::ReadOnly))
			{
				qDebug() << "Raw file handler: " << filePath;

				QByteArray data = existFile.readAll();

				QMimeDatabase db;
				QMimeType mime = db.mimeTypeForFileNameAndData(existFile.fileName(), data);

				qDebug() << "Raw file type is: " << mime.name();

				WebDataContainer webData(data, mime.name());
				QFileInfo info(existFile);
				webData.setModificatedDate(info.lastModified());

				// Save in cache
				WebServer::Instance().cache().put(filePath.toStdString(), webData);

				QString controlEtag = header["If-None-Match"];
				QString eTag = webData.eTag();

				if (controlEtag.size() > 0 && controlEtag == eTag)
				{
					qDebug() << "Requested cache data is good, sending only header";
					socket->write(htmlHeaderGen(QString(), -1, "304 Not Modified"));
				}
				else
				{
					socket->write(htmlHeaderGen(webData));
					socket->write(data);
				}

				return true;
			}
		}
	}

	return false;
}


}
