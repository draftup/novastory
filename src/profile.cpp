#include "profile.h"
#include "utils/globals.h"
#include <QDebug>
#include <QFile>
#include "logic/user.h"
#include "webserver/webserver.h"

namespace novastory
{

	Profile::Profile( const User& target, const User& self /*= User()*/ ) : tragetUser(target), selfUser(self)
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
		selfUser.substitute(data, "my");
		tragetUser.substitute(data, "their");

		return data.toUtf8();
	}

}
