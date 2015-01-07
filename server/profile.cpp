#include "profile.h"
#include "webserver/globals.h"
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

	QString subscriptions;
	for (User& subscription : tragetUser->subscriptions())
	{
		QString name = !subscription.firstName().isEmpty() ? subscription.firstName() + " " + subscription.lastName() : subscription.nickName();
		subscriptions += QString("<div class=\"userava\"><a href=\"/%3\"><img src=\"/avatar/%1\" alt=\"%2\" /></a></div>\n").arg(subscription.userid()).arg(name).arg(subscription.profileId());
	}
	data = data.replace(QString("{their.users.subscriptions_block}"), subscriptions);

	QString subscribed;
	for (User& subscription : tragetUser->subscribed())
	{
		QString name = !subscription.firstName().isEmpty() ? subscription.firstName() + " " + subscription.lastName() : subscription.nickName();
		subscribed += QString("<div class=\"userava\"><a href=\"/%3\"><img src=\"/avatar/%1\" alt=\"%2\" /></a></div>\n").arg(subscription.userid()).arg(name).arg(subscription.profileId());
	}
	data = data.replace(QString("{their.users.subscribed_block}"), subscribed);

	return data.toUtf8();
}

}
