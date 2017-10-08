#include "rawfilehandler.h"
#include "globals.h"
#include "webserver.h"
#include <QDebug>
#include <QFile>
#include <QTcpSocket>

#include <QMimeDatabase>
#include <QMimeType>
#include <QFileInfo>
#include "templator.h"
#include <QDir>

namespace novastory
{

bool RawFileHandler::handle(QTcpSocket* socket, const QString& type, const QString& path, const QHash<QString, QString>& post /* = QHash<QString, QString>() */, const QHash<QString, QString>& get /* = "" */,
							const QHash<QString, QString>& header /*= QHash<QString, QString>()*/, const QHash<QString, QString>& cookies /*= QHash<QString, QString>()*/)
{	
	const QString workingDirectory = WebServer::Instance().directory();
	QString pathReal = path;
	if (path == "/vsteams.js")
	{
		QDir dir(workingDirectory + "/static/js");
		auto files = dir.entryList(QStringList{"main.*.js"});
		if (files.size() > 0)
		{
			QString mainJS = files.first();
			pathReal = "/static/js/" + mainJS;
		}
	}
	if (path == "/vsteams.css")
	{
		QDir dir(workingDirectory + "/static/css");
		auto files = dir.entryList(QStringList{ "main.*.css" });
		if (files.size() > 0)
		{
			QString mainCSS = files.first();
			pathReal = "/static/css/" + mainCSS;
		}
	}

	QString filePath = workingDirectory + pathReal;

	QFile existFile(filePath);
	if (existFile.exists())
	{
		// сервер может передать требуемый язык
		QString needLang;
		if (!get["lang"].isEmpty()) // могут быть языки
		{
			if (WebServer::Instance().languageList().contains(get["lang"]))
			{
				needLang = get["lang"];
			}
		}

		// First, looking in cache
		try
		{
			if (filePath.endsWith(".js") || filePath.endsWith(".css") || filePath.endsWith(".html"))
			{
#if defined(NOVASTORY_BUILD) || defined(VSTEAMS_BUILD)
				if (cookies.contains("userid"))
				{
					qDebug() << "Trying to found required language in current user";
					checkUserLanguage(cookies["userid"].toInt(), cookies["stoken"]);
				}
#endif
#if defined(QT_DEBUG) || defined(REMOTE_TESTING)
				throw std::range_error("debuging");
#endif
			}

			QString postfix;
			QString pref_lang = needLang.isNull() ? WebServer::Instance().defaultLanguage() : needLang;
			if (!pref_lang.isEmpty() && (filePath.endsWith(".js") || filePath.endsWith(".css") || filePath.endsWith(".html")))
			{
				postfix = "-" + pref_lang;
				qDebug() << "Using postfix for this file:" << (filePath + postfix);
			}
			// Вынимаем файл из кэша, притом файлы языкозависимые имеют постфикс uruchie.js-ru и т.п.
			WebDataContainer inCacheData = WebServer::Instance().cache().get((filePath + postfix).toStdString());
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
				socket->write(htmlData(inCacheData, "200 OK", QString(), header));
			}

			return true;
		}
		catch (std::range_error&)
		{
			if (existFile.open(QIODevice::ReadOnly))
			{
				qDebug() << "Raw file handler: " << filePath;

				QByteArray data = existFile.readAll();

				QMimeDatabase db;
				QMimeType mime = db.mimeTypeForFileNameAndData(existFile.fileName(), data);

				qDebug() << "Raw file type is: " << mime.name();

				QString postfix;
				if ((filePath.endsWith(".js") || filePath.endsWith(".css") || filePath.endsWith(".html")) && !filePath.contains("/doc"))
				{
					QString pref_lang = needLang.isNull() ? WebServer::Instance().defaultLanguage() : needLang;
					if (!pref_lang.isEmpty())
					{
						postfix = "-" + pref_lang;
						qDebug() << "Translate file" << filePath + postfix;
					}
					else
					{
						qDebug() << "Clean translations in file" << filePath;
					}
					QString temp = data;
					Templator::translate(temp, pref_lang);
					data = temp.toUtf8();
				}

				WebDataContainer webData(data, mime.name());
				QFileInfo info(existFile);
				webData.setModificatedDate(info.lastModified());

				// Save in cache
				WebServer::Instance().cache().put((filePath + postfix).toStdString(), webData);

				QString controlEtag = header["If-None-Match"];
				QString eTag = webData.eTag();

				if (controlEtag.size() > 0 && controlEtag == eTag)
				{
					qDebug() << "Requested cache data is good, sending only header";
					socket->write(htmlHeaderGen(QString(), -1, "304 Not Modified"));
				}
				else
				{
					socket->write(htmlData(webData, "200 OK", QString(), header));
				}

				return true;
			}
		}
	}

	return false;
}

#if defined(NOVASTORY_BUILD) || defined(VSTEAMS_BUILD)
bool RawFileHandler::checkUserLanguage(const int quserid, const QString& token)
{
	QStringList tokenList = token.split("-");
	if (tokenList.size() != 2)
	{
		return false;
	}

	QString tokenDate = tokenList[0];
	QString tokenKey = tokenList[1];

	novastory::SqlQuery query;
	query.prepare("SELECT password, salt, email, language FROM users WHERE userid = :userid");
	query.bindValue(":userid", quserid);
	VERIFY(query.exec());
	if (query.size() != 1)
	{
		return false;
	}
	VERIFY(query.next());

	QString qsalt = query.value("salt").toString();
	QString qpassword = query.value("password").toString();
	QString qlanguage = query.value("language").toString();

	if (novastory::sha1(tokenDate + novastory::sha1(QString::number(quserid) + novastory::sha1(qsalt) + novastory::sha1("degitx-jelu-leparusvega"))) != tokenKey)
	{
		return false;
	}

	// Устанавливаем язык интерфейса
	if (!qlanguage.isEmpty())
	{
		WebServer::Instance().addDefaultLanguage(qlanguage);
	}

	return true;
}
#endif

}
