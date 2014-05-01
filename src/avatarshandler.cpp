#include "avatarshandler.h"
#include "utils/globals.h"
#include <QDebug>
#include <QTcpSocket>
#include "logic/avatar.h"
#include "webserver/webserver.h"

namespace novastory
{

AvatarsHandler::AvatarsHandler(QTcpSocket* bindedSocket) :
	socket(bindedSocket)
{
}

AvatarsHandler::~AvatarsHandler()
{

}

bool AvatarsHandler::handle(const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */,
							const QHash<QString, QString>& header /*= QHash<QString, QString>()*/, const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/)
{
	if (path.startsWith("/avatar/"))
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
			QString someavatar = path.mid((int)strlen("/avatar/"));
			int id = someavatar.toInt();
			Avatar avatar;
			if (id > 0)
			{
				avatar.setUserid(id);
				if (avatar.sync())
				{
					WebDataContainer newAvatar(avatar.avatar(), avatar.contentType());
					newAvatar.setModificatedDate(avatar.modifyDate());
					WebServer::Instance().cache().put(path.toStdString(), newAvatar);

					QString controlEtag = header["If-None-Match"];
					QString eTag = newAvatar.eTag();

					if (controlEtag.size() > 0 && controlEtag == eTag)
					{
						qDebug() << "Requested cache data is good, sending only header";
						socket->write(htmlHeaderGen(QString(), -1, "304 Not Modified"));
					}
					else
					{
						socket->write(htmlHeaderGen(newAvatar));
						socket->write(newAvatar);
					}
				}
				return true;
			}
			if (!someavatar.contains("@"))
			{
				return false; // who knows what it is
			}

			avatar.setEmail(someavatar);
			if (avatar.sync())
			{
				WebDataContainer newAvatar(avatar.avatar(), avatar.contentType());
				newAvatar.setModificatedDate(avatar.modifyDate());
				WebServer::Instance().cache().put(path.toStdString(), newAvatar);

				socket->write(htmlHeaderGen(newAvatar));
				socket->write(newAvatar);
			}
			return true;
		}
	}

	return false;
}


}
