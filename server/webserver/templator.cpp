#include <QFile>

#include "templator.h"
#include "webserver.h"


namespace novastory
{

QByteArray Templator::generate(
	const QString& title /*= "Novastory"*/,
	const QString& article /*= QString()*/,
	const QHash<QString, QString>& add_map /* = QHash<QString, QString>()*/
)
{
	static QString templateData;
	QString generatedTemplate;

#ifndef QT_DEBUG
	if (templateData.isEmpty())
	{
#endif
		const QString workingDirectory = WebServer::Instance().directory();
		QFile templateFile(workingDirectory + "/template.html");
		if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qCritical() << "Template not founded";
			return QByteArray();
		}

		templateData = templateFile.readAll();
#ifndef QT_DEBUG
	}
#endif


	//replace data
	generatedTemplate = templateData.replace("{title}", title);
	generatedTemplate = generatedTemplate.replace("{article}", article);
	generatedTemplate = generatedTemplate.replace("{powered}", "2014 &copy; Copyright Novastory Engine " GIT_DESCRIBE " [r" GIT_REVISION "]");
	QHashIterator<QString, QString> it(add_map);
	while (it.hasNext())
	{
		it.next();
		generatedTemplate = generatedTemplate.replace("{" + it.key() + "}", it.value());
	}

	qDebug() << "Html template generated with title:" << title << "and article" << article;

	return generatedTemplate.toUtf8();
}

#if defined(NOVASTORY_BUILD) || defined(VSTEAMS_BUILD)
QByteArray Templator::generate(const User& user, const QString& title /*= "Novastory"*/, const QString& article /*= QString() */, const QHash<QString, QString>& add_map /* = QHash<QString, QString>()*/)
{
	if (user.isLogined())
	{
		return generateLogined(user, title, article, add_map);
	}
	else
	{
		return generate(title, article, add_map);
	}
}

QByteArray Templator::generateLogined(const User& user, const QString& title /*= "Novastory"*/, const QString& article /*= QString() */, const QHash<QString, QString>& add_map /* = QHash<QString, QString>()*/)
{
	static QString templateData;
	QString generatedTemplate;

#ifndef QT_DEBUG
	if (templateData.isEmpty())
	{
#endif
		const QString workingDirectory = WebServer::Instance().directory();
		QFile templateFile(workingDirectory + "/template-logined.html");
		if (!templateFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qCritical() << "Template not founded";
			return QByteArray();
		}

		templateData = templateFile.readAll();
#ifndef QT_DEBUG
	}
#endif


	//replace data
	generatedTemplate = templateData.replace("{title}", title);
	generatedTemplate = generatedTemplate.replace("{article}", article);
	generatedTemplate = generatedTemplate.replace("{powered}", "2014 &copy; Copyright Novastory Engine " GIT_DESCRIBE " [r" GIT_REVISION "]");
	user.substitute(generatedTemplate);
	generatedTemplate = generatedTemplate.replace("{users.stoken}", user.token());
#ifdef NOVASTORY_BUILD
	generatedTemplate = generatedTemplate.replace("{users.namemail}", !user.firstName().isEmpty() ? user.firstName() : user.email());
#endif
	QHashIterator<QString, QString> it(add_map);
	while (it.hasNext())
	{
		it.next();
		generatedTemplate = generatedTemplate.replace("{" + it.key() + "}", it.value());
	}

	qDebug() << "Html template generated with title:" << title << "and article" << article;

	return generatedTemplate.toUtf8();
}
#endif

}

