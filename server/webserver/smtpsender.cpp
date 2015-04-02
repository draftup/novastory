#include "smtpsender.h"
#include <QDebug>
#include <SmtpMime>
#include "webserver_config.h"
#include "globals.h"
#include <QCoreApplication>

namespace novastory
{

	SmtpSender::SmtpSender(const QString& tto, const QString& tsubject, const QString& tmessage, const QList<QFile*>& tattachments) : QThread()
{
	setObjectName("Smtp Thread");

	disable_log = false;
	to = tto;
	subject = tsubject;
	message = tmessage;
	attachments = tattachments;

	VERIFY(connect(this, &SmtpSender::finished, &SmtpSender::deleteLater));
	moveToThread(QCoreApplication::instance()->thread());
}

SmtpSender::~SmtpSender()
{
	if (!disable_log)
		qDebug() << "Smtp thread destroyed";
}


void SmtpSender::run()
{
	if (!disable_log)
		qDebug() << "Smtp thread started";
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
	text.setText(this->message);

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

	if (!disable_log)
	{
		qDebug() << "Email sended to " << to << " with subject: " << subject << " and content: " << this->message;
		qDebug() << "Smtp thread finished";
	}
}

void SmtpSender::setLog(bool en)
{
	disable_log = en;
}

}
