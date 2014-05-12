#include "profile.h"
#include "utils/globals.h"
#include <QDebug>
#include <QFile>
#include "logic/user.h"
#include "webserver/webserver.h"

namespace novastory
{

Profile::Profile(User* target, User* self) : tragetUser(target), selfUser(self)
{

}

QByteArray Profile::html() const
{
	const QString workingDirectory = WebServer::Instance().directory();
	QFile templateFile(workingDirectory + "/profile.html");
	if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCritical() << "Template not founded";
		return QByteArray();
	}

	QString data = templateFile.readAll();
	selfUser->substitute(data, "my");
	tragetUser->substitute(data, "their");

	// Other fields
	data = data.replace(QString("{their.users.subscriptions_count}"), QString::number(tragetUser->subscriptions().size()));
	data = data.replace(QString("{their.users.subscribed_count}"), QString::number(tragetUser->subscribed().size()));

	return data.toUtf8();
}

}
