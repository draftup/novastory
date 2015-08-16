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
}


QString WebRouter::path() const
{
	return parsedValues["path"];
}

void WebRouter::sendHtml()
{
	bool isHandeled = false;
	ErrorHandler* errorHandler = nullptr;
	for (QSharedPointer<DataHandler> handler : WebServer::Instance().handlers)
	{
		isHandeled |= handler->handle(bindedSocket, parsedValues["type"], path(), postVariables, QString(), parsedValues, cookieVariables);
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
			VERIFY(errorHandler->handle(bindedSocket, parsedValues["type"], path(), postVariables, QString(), parsedValues, cookieVariables, 404));
		}
	}
}

QString WebRouter::postData() const
{
	return parsedValues["POST"];
}

void WebRouter::parsePost()
{
	QString data = postData();
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

		postVariables.insert(keyValuePair[0], QUrl::fromPercentEncoding(keyValuePair[1].replace(QChar('+'), "%20").toUtf8()));
	}
}

void WebRouter::parse()
{
	WebRequest::parse();
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

	QRegExp rx("([a-z]{1,8})(?:-[a-z]{1,8})?(?:;q=([0-9.]+))?");
	rx.setCaseSensitivity(Qt::CaseInsensitive);
	int pos = 0;
	while ((pos = rx.indexIn(laguages_str, pos)) != -1)
	{
		pos += rx.matchedLength();
		laguages[(rx.cap(2).isEmpty() ? 1.0 : rx.cap(2).toDouble())] = rx.cap(1).toLower();
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
		qDebug() << "Setting up current language for user: " << prefer_lang;
	}
}

}