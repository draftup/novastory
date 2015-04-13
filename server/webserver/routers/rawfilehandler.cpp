#include "rawfilehandler.h"
#include "globals.h"
#include "webserver.h"
#include <QDebug>
#include <QFile>
#include <QTcpSocket>

#include <QMimeDatabase>
#include <QMimeType>
#include <QFileInfo>
#include "templator.h"

namespace novastory
{

bool RawFileHandler::handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */,
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
#if defined(QT_DEBUG) || defined(REMOTE_TESTING)
			if (filePath.endsWith(".js") || filePath.endsWith(".css") || filePath.endsWith(".html"))
			{
				throw std::range_error("debuging");
			}
#endif
			QString postfix;
			QString pref_lang = WebServer::Instance().defaultLanguage();
			if (!pref_lang.isEmpty() && (filePath.endsWith(".js") || filePath.endsWith(".css") || filePath.endsWith(".html")))
			{
				postfix = "-" + pref_lang;
				qDebug() << "Using postfix for this file:" << (filePath + postfix);
			}
			// Вынимаем файл из кэша, притом файлы языкозависимые имеют постфикс uruchie.js-ru и т.п.
			WebDataContainer inCacheData = WebServer::Instance().cache().get((filePath + postfix).toStdString());
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

				QString postfix;
				QString pref_lang = WebServer::Instance().defaultLanguage();
				if (filePath.endsWith(".js") || filePath.endsWith(".css") || filePath.endsWith(".html"))
				{
					if (!pref_lang.isEmpty())
					{
						postfix = "-" + pref_lang;
						qDebug() << "Translate file" << filePath + postfix;
					}
					else
					{
						qDebug() << "Clean translations in file" << filePath;
					}
					QString temp = data;
					Templator::translate(temp);
					data = temp.toUtf8();
				}

				WebDataContainer webData(data, mime.name());
				QFileInfo info(existFile);
				webData.setModificatedDate(info.lastModified());

				// Save in cache
				WebServer::Instance().cache().put((filePath + postfix).toStdString(), webData);

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
