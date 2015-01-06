#include "globals.h"
#include <time.h>
#include <QCryptographicHash>
#include <SmtpMime>
#include "webserver_config.h"
#include <thread>

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
	SmtpClient smtp(SMTP_SERVER, SMTP_PORT, SmtpClient::SslConnection);
	smtp.setUser(SMTP_USER);
	smtp.setPassword(SMTP_PASSWORD);

	MimeMessage message;
	EmailAddress from_address("uruchie.org@gmail.com");
	EmailAddress to_address(to);
	message.setSender(&from_address);
	message.addRecipient(&to_address);
	message.setSubject(subject);

	MimeText text;
	text.setText(textofmessage);

	message.addPart(&text);
	QList<QSharedPointer<MimeAttachment>> attachToDelete;
	for (QFile* file : attachments)
	{
		QSharedPointer<MimeAttachment> a(new MimeAttachment(file));
		attachToDelete.append(a);
		message.addPart(a.data());
	}

	// Now we can send the mail
	smtp.connectToHost();
	smtp.login();
	smtp.sendMail(message);
	smtp.quit();

	qDebug() << "Email sended to " << to << " with subject: " << subject << " and content: " << textofmessage;
}

void novastory::sendAsyncMail(const QString& to, const QString& subject, const QString& message, const QList<QFile*>& attachments /* = QList<QFile*>()*/)
{
	std::thread([to, subject, message, attachments]()
	{
		qDebug() << "Mail thread openned";
		sendMail(to, subject, message, attachments);
		qDebug() << "Mail thread closed";
	}).detach();
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
