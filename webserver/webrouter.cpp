#include "webrouter.h"
#include "webserver.h"
#include "globals.h"
#include "routers/errorhandler.h"
#include <QTextStream>
#include <QTcpSocket>
#include <QUrl>
#include <QThread>
#include <QMetaObject>
#include <QSet>
#include <QCoreApplication>

namespace novastory
{
WebRouter::WebRouter(QTcpSocket* bindedSocket) : WebRequest(bindedSocket)
{
	/*

	*/
}


WebRouter::~WebRouter()
{
	WebServer::Instance().removeDefaultLanguage();
#if defined(VSTEAMS_BUILD)
	WebServer::Instance().removePreferedGame();
#endif
}


QString WebRouter::path() const
{
	return parsedValues["path"];
}

void WebRouter::sendHtml()
{
#ifdef QT_DEBUG
	if (path() == "/exit")
	{
		QCoreApplication::instance()->quit();
		return;
	}
#endif
#if defined(VSTEAMS_BUILD)
	if (!cookieVariables["preferedgame"].isNull())
	{
		WebServer::Instance().setPreferedGame(cookieVariables["preferedgame"].toInt());
	}
#endif
	bool isHandeled = false;
	ErrorHandler* errorHandler = nullptr;
	for (QSharedPointer<DataHandler> handler : WebServer::Instance().handlers)
	{
		isHandeled |= handler->handle(bindedSocket, parsedValues["type"], path(), postVariables, getVariables, parsedValues, cookieVariables);
		if (errorHandler == nullptr)
		{
			errorHandler = dynamic_cast<ErrorHandler*>(handler.data());
		}
	}
	if (!isHandeled)
	{
		qDebug() << "404 Error, page not founded";
		if (errorHandler)
		{
			VERIFY(errorHandler->handle(bindedSocket, parsedValues["type"], path(), postVariables, getVariables, parsedValues, cookieVariables, 404));
		}
	}
}

QString WebRouter::postData() const
{
	return parsedValues["POST"];
}

QString WebRouter::getData() const
{
	return parsedValues["GET"];
}

void WebRouter::parseParams(const QString& data, QHash<QString, QString>* var)
{
	if (data.isEmpty())
	{
		return;
	}

	for (QString pair : data.split("&"))
	{
		QStringList keyValuePair = pair.split("=");

		if (keyValuePair.size() != 2)
		{
			continue;
		}

		var->insert(keyValuePair[0], QUrl::fromPercentEncoding(keyValuePair[1].replace(QChar('+'), "%20").toUtf8()));
	}
}

void WebRouter::parsePost()
{
	QString data = postData();
	parseParams(data, &postVariables);
}

void WebRouter::parseGet()
{
	QString data = getData();
	parseParams(data, &getVariables);
}

void WebRouter::parse()
{
	WebRequest::parse();
	parseGet();
	parsePost();
	parseCookie();
	parseLanguage();
}

void WebRouter::parseCookie()
{
	QString data = parsedValues["Cookie"];
	if (data.isEmpty())
	{
		return;
	}

	for (QString pair : data.split("; "))
	{
		QStringList keyValuePair = pair.split("=");

		if (keyValuePair.size() != 2)
		{
			continue;
		}

		auto element = cookieVariables.insert(keyValuePair[0], keyValuePair[1].trimmed());
		qDebug() << "Parsed cookie " << element.key() << "=" << element.value();
	}
}

QString WebRouter::coockie(const QString& name)
{
	return cookieVariables[name];
}

void WebRouter::parseLanguage()
{
	if (
		cookieVariables.contains("locale")
		&& !cookieVariables["locale"].isEmpty()
		&& WebServer::Instance().languageList().contains(cookieVariables["locale"])
		)
	{
		qDebug() << "Setting up current language for user by cookie: " << cookieVariables["locale"];
		WebServer::Instance().addDefaultLanguage(cookieVariables["locale"]);
		return;
	}

	if (!parsedValues.contains("Accept-Language"))
	{
		qDebug() << "No Accept-Language in this client. Ignore language setting.";
		return;
	}

	QString laguages_str = parsedValues["Accept-Language"];

	if (laguages_str.isEmpty())
	{
		qWarning() << "Very stange. Accept-Language recived bu empty";
		return;
	}

	QMap<double, QString> laguages;

	QRegExp rx("([a-z]{1,8})(?:-[a-zA-Z]{1,8})?(?:;q=([0-9.]+))?");
	rx.setCaseSensitivity(Qt::CaseInsensitive);
	int pos = 0;
	double current_priority = 1.0;
	while ((pos = rx.indexIn(laguages_str, pos)) != -1)
	{
		pos += rx.matchedLength();
		laguages[(rx.cap(2).isEmpty() ? current_priority : rx.cap(2).toDouble())] = rx.cap(1).toLower();
		if(rx.cap(2).isEmpty())
			current_priority -= .0001;
	}
	qDebug() << "User client languages are:" << laguages.size();
	QSet<QString> available_langs = WebServer::Instance().languageList().toSet();
	QMapIterator<double, QString> it(laguages);
	it.toBack();
	QString prefer_lang;
	while (it.hasPrevious())
	{
		it.previous();
		if (it.key() <= 0.5 && !prefer_lang.isNull())
		{
			break;
		}

		if (available_langs.contains(it.value()))
		{
			prefer_lang = it.value();
		}

		if (!prefer_lang.isNull() && prefer_lang != "en")
		{
			break;
		}
	}
	if (!prefer_lang.isNull())
	{
		WebServer::Instance().addDefaultLanguage(prefer_lang);
		cookieVariables["locale"] = prefer_lang;
		qDebug() << "Setting up current language for user: " << prefer_lang;
	}
}

}