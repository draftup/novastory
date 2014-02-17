#include "captcha.h"
#include <QVariant>
#include <QDebug>
#include "utils/globals.h"
#include "sql/sqlquery.h"
#include "recaptcha.h"


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

const QString& novastory::Captcha::username() const
{
	return m_username;
}

void novastory::Captcha::setUsername(const QString& username)
{
	m_username = username;
}

novastory::Captcha::Captcha()
{
	setObjectName("usersverify");
}

bool novastory::Captcha::addVerifyNotify()
{
	if(m_username.isEmpty() || m_password.isEmpty() || m_email.isEmpty() || m_challenge.isEmpty() || m_response.isEmpty() || m_remoteIP.toString().isEmpty())
	{
		qDebug() << "Empty captcha info";
		return false;
	}
	if(m_username.length() < 2 || m_username.length() > 100)
	{
		qDebug() << "Username size not correct";
		return false;
	}
	if(m_email.indexOf("@") == -1)
	{
		qDebug() << "Email for user " << m_username << " not correct";
		return false;
	}
	if(m_password.length() != 32)
	{
		qDebug() << "Password for user " << m_username << "not correct";
		return false;
	}

	SqlQuery query;
	query.prepare("SELECT userid FROM users WHERE email = :email OR username = :username");
	query.bindValue(":email", m_email);
	query.bindValue(":username", m_username);
	VERIFY(query.exec());
	if(query.size() > 0)
	{
		qDebug() << m_username << " already exist in database with such email or username";
		return false;
	}

	setToken(generateToken());

	Recaptcha captchaChecker(m_challenge, m_response, m_remoteIP);
	if(!captchaChecker.checkCaptchaSync())
	{
		qDebug() << m_username << " doesn't pass captcha verification";
		return false;
	}

	return insertSQL();
}

QString novastory::Captcha::generateToken() const
{
	return md5(md5(m_username) + md5(m_password) + md5(m_email) + unixtime());
}

bool novastory::Captcha::syncByToken(const QString& token)
{
	setToken(token);
	if(!m_token.isEmpty())
	{
		return true;
	}
	bool status = syncSQL("token");
	if(!status)
	{
		return false;
	}

	return !m_token.isEmpty();
}

bool novastory::Captcha::deleteByToken( const QString& token /*= QString()*/ )
{
	setToken(token);
	if(!m_token.isEmpty())
	{
		return true;
	}
	return removeSQL("token");
}
