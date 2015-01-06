#include "userspicshandler.h"
#include "utils/globals.h"
#include <QDebug>
#include <QTcpSocket>
#include "logic/userpic.h"
#include "webserver/webserver.h"

namespace novastory
{

bool UsersPicsHandler::handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */,
							  const QHash<QString, QString>& header /*= QHash<QString, QString>()*/, const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/)
{
	if (path.startsWith("/userpic/"))
	{
		try
		{
			WebDataContainer inCacheData = WebServer::Instance().cache().get(path.toStdString());
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
			QString someavatar = path.mid((int)strlen("/userpic/"));
			int id = someavatar.toInt();
			UserPic userpic;
			if (id > 0)
			{
				userpic.setUserid(id);
				if (userpic.sync())
				{
					WebDataContainer userPicData = userpic.userPic();
					WebServer::Instance().cache().put(path.toStdString(), userPicData);

					QString controlEtag = header["If-None-Match"];
					QString eTag = userPicData.eTag();

					if (controlEtag.size() > 0 && controlEtag == eTag)
					{
						qDebug() << "Requested cache data is good, sending only header";
						socket->write(htmlHeaderGen(QString(), -1, "304 Not Modified"));
					}
					else
					{
						socket->write(htmlHeaderGen(userPicData));
						socket->write(userPicData);
					}
				}
				return true;
			}
			if (!someavatar.contains("@"))
			{
				return false; // who knows what it is
			}

			userpic.setEmail(someavatar);
			if (userpic.sync())
			{
				WebDataContainer userPicData = userpic.userPic();
				WebServer::Instance().cache().put(path.toStdString(), userPicData);

				socket->write(htmlHeaderGen(userPicData));
				socket->write(userPicData);
			}
			return true;
		}
	}

	return false;
}

}
