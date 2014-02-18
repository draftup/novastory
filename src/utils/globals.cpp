#include "globals.h"
#include <time.h>
#include <QCryptographicHash>
#include <SmtpMime>
#include "config.h"
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

void novastory::sendMail( const QString& to, const QString& subject, const QString& textofmessage )
{
	SmtpClient smtp(SMTP_SERVER, SMTP_PORT, SmtpClient::SslConnection);
	smtp.setUser(SMTP_USER);
	smtp.setPassword(SMTP_PASSWORD);
	
	MimeMessage message;
	message.setSender(new EmailAddress("uruchie.org@gmail.com"));
	message.addRecipient(new EmailAddress(to));
	message.setSubject(subject);

	MimeText text;
	text.setText(textofmessage);

	message.addPart(&text);

	// Now we can send the mail
	smtp.connectToHost();
	smtp.login();
	smtp.sendMail(message);
	smtp.quit();

}

void novastory::sendAsyncMail( const QString& to, const QString& subject, const QString& message )
{
	std::thread mailThread([&](){
		sendMail(to, subject, message);
	});
}
