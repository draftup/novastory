#ifndef RECAPTCHA_H
#define RECAPTCHA_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QHostAddress>
#include <QNetworkReply>
#include "config.h"

class QNetworkReply;

namespace novastory
{

class Recaptcha : public QObject
{
	Q_OBJECT
public:
	Recaptcha(QString challenge = QString(), QString response = QString(), QHostAddress ip = QHostAddress());
	~Recaptcha();
	void checkCaptchaASync();
	bool checkCaptchaSync();
private:
	QNetworkAccessManager manager;
	QString challenge;
	QString response;
	QHostAddress remoteIpAdress;
	int result;
protected slots:
	void processReply(QNetworkReply* reply);
private slots:
	void onNetworkError(QNetworkReply::NetworkError);
signals:
	void verifyed(bool status);
};


}

#endif
