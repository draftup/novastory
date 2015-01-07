#include "profilehandler.h"
#include "webserver/globals.h"
#include <QDebug>
#include <QTcpSocket>
#include "logic/user.h"
#include "profile.h"
#include "webserver/templator.h"

namespace novastory
{

bool ProfileHandler::handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QString& get /* = "" */,
							const QHash<QString, QString>& header /* = QHash<QString, QString>()*/, const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/)
{
	if (path.startsWith("/") && User::profileRegExp().exactMatch(path.mid(1)))
	{
		User userProfile;
		userProfile.setProfileId(path.mid(1));
		if (!userProfile.sync())
		{
			return false;
		}

		User loginedUser;
		if (cookies.contains("userid"))
		{
			loginedUser.loginByToken(cookies["userid"].toInt(), cookies["stoken"]);
		}

		Profile profileBuilder(&userProfile, &loginedUser);
		QString profileElement = selectorId(profileBuilder.html(), "propage");

		QString title = userProfile.nickName();
		if (title.isEmpty())
		{
			title = userProfile.firstName() + " " + userProfile.lastName();
		}
		if (title.size() == 1)
		{
			title = userProfile.email();
		}
		title += " profile";

		QByteArray responce = Templator::generate(loginedUser, title, profileElement);
		socket->write(htmlHeaderGen("text/html", responce.size()));
		socket->write(responce);
	}

	return false;
}


}
