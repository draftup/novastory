#include "globals.h"
#include <time.h>
#include <QCryptographicHash>
#include "smtpsender.h"
#include "webserver_config.h"
#include "webserver.h"
#include <QTranslator>
#include "translatorhelper.h"
#include <QThread>
#include <zlib.h>

QString novastory::md5(const QString& str)
{
	return QString(QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5).toHex());
}

QString novastory::sha1(const QString& str)
{
	return QString(QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Sha1).toHex());
}

qint64 novastory::unixtime()
{
	return static_cast<qint64>(time(NULL));
}

void novastory::sendMail(const QString& to, const QString& subject, const QString& textofmessage, const QList<QFile*>& attachments /* = QList<QFile*>()*/, bool disable_log)
{
	SmtpSender sender(to, subject, textofmessage, attachments);
	sender.setLog(disable_log);
	sender.run();
}

void novastory::sendAsyncMail(const QString& to, const QString& subject, const QString& message, const QList<QFile*>& attachments /* = QList<QFile*>()*/)
{
	SmtpSender* sender = new SmtpSender(to, subject, message, attachments);
	sender->start();
}

QString novastory::selectorId(const QString& html, const QString& selector)
{
	QHash<QString, int> tags;
	QRegExp idTag("<([^\\s>]+)[^>]*id=['\"]" + selector + "['\"][^>]*>");
	QRegExp simpleTag("</?([^\\s>/]+)[^>]*>");
	int begin = 0, end = html.length(), pos = 0;
	if ((pos = idTag.indexIn(html, pos)) != -1)
	{
		tags[idTag.cap(1)]++;
		begin = pos;
		pos += idTag.matchedLength();
		while (tags[idTag.cap(1)] > 0 && (pos = simpleTag.indexIn(html, pos)) != -1)
		{
			// Пропуск комментариев
			if (simpleTag.cap(0).startsWith("<!--"))
			{
				int end_comment = html.indexOf("-->", pos);
				if (end_comment > 0)
				{
					pos = end_comment + 3;
				}
				else
				{
					pos += 4;
				}

				end = pos;
				continue;
			}

			if (simpleTag.cap(0).startsWith("</"))
			{
				tags[simpleTag.cap(1)]--;
			}
			else
			{
				tags[simpleTag.cap(1)]++;
			}
			pos += simpleTag.matchedLength();
			end = pos;
		}
	}

	return html.mid(begin, end - begin);
}

QString novastory::tr(const QString& key, const QString& dis /*= QString()*/, const QString& default_language /* = QString() */)
{
	return translate("novastory", key, dis, default_language);
}

QString novastory::translate(const QString& context, const QString& key, const QString& dis /*= QString()*/, const QString& default_language /* = QString() */)
{
	QString lang = default_language.isNull() ? WebServer::Instance().defaultLanguage() : default_language;

	if (lang.isEmpty())
	{
		return key;
	}

	if (!WebServer::Instance().webTranslators().contains(lang))
	{
		return key;
	}

#ifdef GENERATE_TRANSLATIONS
	WebServer::Instance().webTranslatorsHelper().appendTranslation(key, dis, context);
#endif
	QString ret = WebServer::Instance().webTranslators().value(lang)->translate(context.toLatin1().constData(), key.toLatin1().constData(), dis.toLatin1().constData());
	if (ret.isNull())
	{
		return key;
	}

	return ret;
}

QByteArray novastory::htmlData(const QByteArray& data, const QString& mimetype /*= QString()*/, const QString& status /*= "200 OK"*/, const QString& additional /*= QString()*/, const QHash<QString, QString>& header /*= QHash<QString, QString>()*/)
{
	// Компресия deflate
	QByteArray mData = data;
	QString addAdditional = additional;
	if (header["Accept-Encoding"].contains("deflate"))
	{
		ulong len = data.size() + data.size() / 100 + 12;
		mData.resize(0);
		int res;
		do
		{
			mData.resize(len);
			res = ::compress2((uchar*)mData.data(), &len, (uchar*)data.constData(), data.size(), -1);

			switch (res)
			{
			case Z_OK:
				mData.resize(len);
				break;
			case Z_MEM_ERROR:
				qWarning("deflate error: Z_MEM_ERROR: Not enough memory");
				mData.resize(0);
				break;
			case Z_BUF_ERROR:
				len *= 2;
				break;
			}
		}
		while (res == Z_BUF_ERROR);
		addAdditional += "Content-Encoding: deflate\n";
	}
	return htmlHeaderGen(mimetype, mData.size(), status, addAdditional) + mData;
}

QByteArray novastory::htmlData(const WebDataContainer& data, const QString& status /*= "200 OK"*/, const QString& additional /*= QString()*/, const QHash<QString, QString>& header /*= QHash<QString, QString>()*/)
{
	// Компресия deflate
	WebDataContainer mData = data;
	QString addAdditional = additional;
	if (header["Accept-Encoding"].contains("deflate"))
	{
		ulong len = data.size() + data.size() / 100 + 12;
		mData.resize(0);
		int res;
		do
		{
			mData.resize(len);
			res = ::compress2((uchar*)mData.data(), &len, (uchar*)data.constData(), data.size(), -1);

			switch (res)
			{
			case Z_OK:
				mData.resize(len);
				break;
			case Z_MEM_ERROR:
				qWarning("deflate error: Z_MEM_ERROR: Not enough memory");
				mData.resize(0);
				break;
			case Z_BUF_ERROR:
				len *= 2;
				break;
			}
		}
		while (res == Z_BUF_ERROR);
		addAdditional += "Content-Encoding: deflate\n";
	}
	return htmlHeaderGen(mData, status, addAdditional) + mData;
}
