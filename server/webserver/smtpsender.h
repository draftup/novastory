#ifndef SMTPSENDER_H
#define SMTPSENDER_H

#include <QThread>
#include <QString>
#include <QFile>

class Test_Cron;

namespace novastory
{

class SmtpSender : public QThread
{
	Q_OBJECT
public:
	SmtpSender(const QString& to, const QString& subject, const QString& message, const QList<QFile*>& attachments = QList<QFile*>());
	~SmtpSender();
	void run() override;
private:
	QString to;
	QString subject;
	QString message;
	QList<QFile*> attachments;
};

}

#endif
