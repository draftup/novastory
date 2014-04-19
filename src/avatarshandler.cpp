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
							const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/)
{
	if (path.startsWith("/avatar/"))
	{
		try
		{
			WebDataContainer inCacheData = WebServer::Instance().cache().get(path.toStdString());
			qDebug() << "Readed from cache " << path << "(Current cache size:" << WebServer::Instance().cache().currentSize() << ")";
			socket->write(htmlHeaderGen(inCacheData.mimeType(), inCacheData.size()));
			socket->write(inCacheData);
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
					WebServer::Instance().cache().put(path.toStdString(), WebDataContainer(avatar.avatar(), avatar.contentType()));
					showAvatar(avatar);
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
				WebServer::Instance().cache().put(path.toStdString(), WebDataContainer(avatar.avatar(), avatar.contentType()));
				showAvatar(avatar);
			}
			return true;
		}
	}

	return false;
}

void AvatarsHandler::showAvatar(const Avatar& avatar)
{
	socket->write(htmlHeaderGen(avatar.contentType(), avatar.contentSize()));
	socket->write(avatar.avatar());
}


}
