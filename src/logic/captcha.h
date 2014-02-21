#ifndef	CAPTHA_H
#define CAPTHA_H

#include "sql/sqlizable.h"
#include "jsonthrower.h"
#include <QHostAddress>

class Test_Recaptcha;

namespace novastory
{

class Captcha : protected Sqlizable, public JsonThrower
{
	friend class ::Test_Recaptcha;
	Q_OBJECT
	Q_PROPERTY(QString token READ token WRITE setToken)
	Q_PROPERTY(QString password READ password WRITE setPassword)
	Q_PROPERTY(QString salt READ salt WRITE setSalt)
	Q_PROPERTY(QString email READ email WRITE setEmail)
public:
	Captcha();

	bool addVerifyNotify();

	const QString& token() const;
	void setToken(const QString& token);
	bool syncByToken();
	bool deleteByToken();

	const QString& password() const;
	void setPassword(const QString& password);

	const QString& salt() const;
	void setSalt(const QString& salt);

	const QString& email() const;
	void setEmail(const QString& email);

	const QString& challenge() const { return m_challenge; }
	void setChallenge(const QString& challenge) { m_challenge = challenge; }

	const QString& response() const { return m_response; }
	void setResponse(const QString& response) { m_response = response; }

	const QHostAddress& remoteIP() const { return m_remoteIP; }
	void setRemoteIP(const QHostAddress& ip) { m_remoteIP = ip; }
protected:
	QString generateToken() const;

	void sendVerificaionMail();
private:
	QString m_token;
	QString m_password;
	QString m_salt;
	QString m_email;

	QString m_challenge;
	QString m_response;
	QHostAddress m_remoteIP;
};

}

#endif
