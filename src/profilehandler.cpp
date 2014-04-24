#include "profilehandler.h"
#include "utils/globals.h"
#include <QDebug>
#include <QTcpSocket>
#include "logic/user.h"

namespace novastory
{

ProfileHandler::ProfileHandler(QTcpSocket* bindedSocket) :
	socket(bindedSocket)
{
}

ProfileHandler::~ProfileHandler()
{

}

bool ProfileHandler::handle(const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */,
							const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/)
{
	if (path.startsWith("/") && User::profileRegExp().exactMatch(path.mid(1)))
	{
		User userProfile;
		userProfile.setProfileId(path.mid(1));
		if(!userProfile.sync())
			return false;
	}

	return false;
}


}
