#include "captcha.h"
#include <QVariant>
#include <QDebug>
#include "utils/globals.h"
#include "sql/sqlquery.h"
#include "recaptcha.h"
#include "user.h"


const QString& novastory::Captcha::token() const
{
	return m_token;
}

void novastory::Captcha::setToken(const QString& token)
{
	m_token = token;
}


const QString& novastory::Captcha::password() const
{
	return m_password;
}

void novastory::Captcha::setPassword(const QString& password)
{
	m_password = password;
}

const QString& novastory::Captcha::salt() const
{
	return m_salt;
}

void novastory::Captcha::setSalt(const QString& salt)
{
	m_salt = salt;
}

const QString& novastory::Captcha::email() const
{
	return m_email;
}

void novastory::Captcha::setEmail(const QString& email)
{
	m_email = email;
}

novastory::Captcha::Captcha()
{
	setObjectName("usersverify");
}

bool novastory::Captcha::addVerifyNotify()
{
	if (m_password.isEmpty() || m_email.isEmpty() || m_challenge.isEmpty() || m_response.isEmpty() || m_remoteIP.toString().isEmpty())
	{
		JSON_ERROR("Empty captcha info", 1);
		return false;
	}
	if (m_email.indexOf("@") == -1)
	{
		JSON_ERROR("Email for user " + m_email + " not correct", 2);
		return false;
	}
	if (m_password.length() != 40)
	{
		JSON_ERROR("Password for user " + m_email + "not correct", 3);
		return false;
	}

	User saltGenerator;
	saltGenerator.setSHA1Password(m_password);
	setPassword(saltGenerator.password());
	setSalt(saltGenerator.salt());

	SqlQuery query;
	query.prepare("SELECT userid FROM users WHERE email = :email");
	query.bindValue(":email", m_email);
	VERIFY(query.exec());
	if (query.size() > 0)
	{
		JSON_ERROR(m_email + " already exist in database with such email", 4);
		return false;
	}

	setToken(generateToken());

	Recaptcha captchaChecker(m_challenge, m_response, m_remoteIP);
	if (!captchaChecker.checkCaptchaSync())
	{
		JSON_ERROR(m_email + " doesn't pass captcha verification", 5);
		return false;
	}

	sendVerificaionMail();

	return insertSQL();
}

QString novastory::Captcha::generateToken() const
{
	return md5(md5(m_password) + md5(m_email) + unixtime());
}

bool novastory::Captcha::syncByToken()
{
	if (m_token.isEmpty())
	{
		return false;
	}

	return syncSQL("token");
}

bool novastory::Captcha::deleteByToken()
{
	if (m_token.isEmpty())
	{
		return false;
	}

	return removeSQL("token");
}

void novastory::Captcha::sendVerificaionMail()
{
	if (m_token.isEmpty())
	{
		return;
	}

	sendAsyncMail(
		m_email,
		"Welcome to Novastory (novastory.org)",
		"To validate your new account follow the link below:\nhttp://novastory.org/validate/" + m_token + "\n\nGood luck with novastory!"
	);
}
