#include "globals.h"
#include <time.h>
#include <QCryptographicHash>
#include "smtpsender.h"
#include "webserver_config.h"

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

void novastory::sendMail(const QString& to, const QString& subject, const QString& textofmessage, const QList<QFile*>& attachments /* = QList<QFile*>()*/)
{
	SmtpSender sender(to, subject, textofmessage, attachments);
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
