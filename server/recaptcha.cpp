#include "recaptcha.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

#include "webserver/globals.h"
#include "config.h"

namespace novastory
{

Recaptcha::Recaptcha(QString c_challenge /*= QString()*/, QString c_response /*= QString()*/, QHostAddress ip) :
	challenge(c_challenge),
	response(c_response),
	remoteIpAdress(ip),
	result(0)
{
	VERIFY(connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processReply(QNetworkReply*))));
}

Recaptcha::~Recaptcha()
{

}

void Recaptcha::processReply(QNetworkReply* reply)
{
	QStringList data = QString(reply->readAll()).split("\n");
	bool isVerifyed = (data[0] == "true");
	result = isVerifyed ? 1 : -1;
	qDebug() << "ReCaptcha responce for user " << remoteIpAdress.toString() << ":" << isVerifyed;
	emit verifyed(isVerifyed);
	reply->deleteLater();
}

void Recaptcha::checkCaptchaASync()
{
	result = 0; // reset last result
	QUrlQuery params;
	params.addQueryItem("privatekey", RECAPTCHA_PRIVATE_KEY);
	params.addQueryItem("remoteip", remoteIpAdress.toString());
	params.addQueryItem("challenge", challenge);
	params.addQueryItem("response", response);
	QNetworkRequest request(QUrl("http://www.google.com/recaptcha/api/verify"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	QNetworkReply* reply = manager.post(request, params.query(QUrl::FullyEncoded).toUtf8());
	VERIFY(connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onNetworkError(QNetworkReply::NetworkError))));
}

void Recaptcha::onNetworkError(QNetworkReply::NetworkError error)
{
	qCritical() << "Captcha network error: " << error;
}

bool Recaptcha::checkCaptchaSync()
{
	QEventLoop eLoop;
	VERIFY(connect(this, SIGNAL(verifyed(bool)), &eLoop, SLOT(
					   quit())));
	QTimer::singleShot(20000, &eLoop, SLOT(quit())); // exit from event loop by interval
	checkCaptchaASync();
	if (result == 0)
	{
		eLoop.exec();
	}
	if (result == 0)
	{
		qCritical() << "We don't recive any info about recaptcha";
		return false;
	}

	return result > 0;
}

}
