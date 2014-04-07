#include <QFile>

#include "templator.h"
#include "webserver/webserver.h"


namespace novastory
{

QByteArray Templator::generate(
	const QString& title /*= "Novastory"*/,
	const QString& article /*= QString()*/
)
{
	static QString templateData;
	QString generatedTemplate;

	if (templateData.isEmpty())
	{
		const QString workingDirectory = WebServer::Instance().directory();
		QFile templateFile(workingDirectory + "/template.html");
		if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qCritical() << "Template not founded";
			return QByteArray();
		}

		templateData = templateFile.readAll();
	}


	//replace data
	generatedTemplate = templateData.replace("{title}", title);
	generatedTemplate = generatedTemplate.replace("{article}", article);
	generatedTemplate = generatedTemplate.replace("{powered}", "2014 &copy; Copyright Novastory Engine " GIT_DESCRIBE " [r" GIT_REVISION "]");

	qDebug() << "Html template generated with title:" << title << "and article" << article;

	return generatedTemplate.toUtf8();
}

QByteArray Templator::generateLogined( const User& user, const QString& title /*= "Novastory"*/, const QString& article /*= QString() */ )
{
	static QString templateData;
	QString generatedTemplate;

	if (templateData.isEmpty())
	{
		const QString workingDirectory = WebServer::Instance().directory();
		QFile templateFile(workingDirectory + "/template-logined.html");
		if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qCritical() << "Template not founded";
			return QByteArray();
		}

		templateData = templateFile.readAll();
	}


	//replace data
	generatedTemplate = templateData.replace("{title}", title);
	generatedTemplate = generatedTemplate.replace("{article}", article);
	generatedTemplate = generatedTemplate.replace("{powered}", "2014 &copy; Copyright Novastory Engine " GIT_DESCRIBE " [r" GIT_REVISION "]");
	generatedTemplate = generatedTemplate.replace("{user.userid}", QString::number(user.userid()));
	generatedTemplate = generatedTemplate.replace("{user.stoken}", user.token());

	qDebug() << "Html template generated with title:" << title << "and article" << article;

	return generatedTemplate.toUtf8();
}

}

