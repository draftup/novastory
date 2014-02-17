#include "user.h"
#include <QVariant>
#include <QDebug>
#include "utils/globals.h"
#include "sql/sqlquery.h"
#include "logic/captcha.h"

int novastory::User::userid() const
{
	return m_userid;
}

void novastory::User::setUserID(int userid)
{
	m_userid = userid;
}

void novastory::User::resetUserid()
{
	m_userid = -1;
}

const QString& novastory::User::password() const
{
	return m_password;
}

void novastory::User::setPassword(const QString& password)
{
	m_password = password;
}

void novastory::User::resetPassword()
{
	m_password = QString();
}

const QString& novastory::User::salt() const
{
	return m_salt;
}

void novastory::User::setSalt(const QString& salt)
{
	m_salt = salt;
}

void novastory::User::resetSalt()
{
	m_salt = QString();
}


const QString& novastory::User::email() const
{
	return m_email;
}

void novastory::User::setEmail(const QString& email)
{
	m_email = email;
}

void novastory::User::resetEmail()
{
	m_email = QString();
}

const QString& novastory::User::username() const
{
	return m_username;
}

void novastory::User::setUsername(const QString& username)
{
	m_username = username;
}

void novastory::User::resetUsername()
{
	m_username = QString();
}

novastory::User::User() : m_userid(-1)
{
	setObjectName("users");
	setProperty("auto_increment", QVariant("userid"));
	setProperty("primary_key", QVariant("userid"));
}

void novastory::User::setRawPassword(const QString& password)
{
	QString currentSalt = salt();

	if (currentSalt.isEmpty())
	{
		// if old salt is empty we must generate new salt
		currentSalt = generateSalt();
		setSalt(currentSalt);
	}

	setPassword(md5(sha1(password) + sha1(currentSalt) + "novastory"));
}

bool novastory::User::addUser()
{
	if (m_username.isEmpty() || m_password.isEmpty() || m_salt.isEmpty() || m_email.isEmpty())
	{
		return false;    // something is empty
	}

	// Checking if user already exist
	SqlQuery query;
	query.prepare("SELECT userid FROM users WHERE email = ? OR username = ?");
	query.bindValue(0, m_email);
	query.bindValue(1, m_username);
	VERIFY(query.exec());

	if (query.size() != 0)
	{
		qDebug() << "User " <<  m_username << "(" << m_email << ") already in database";
		return false;
	}

	return insertSQL();
}

QString novastory::User::generateSalt() const
{
	return md5(QString::number(unixtime()) + "salt" + (rand() % 256));
}

novastory::User* novastory::User::verifyUser(const QString& token)
{
	Captcha capthaCheck;
	capthaCheck.setToken(token);
	if (!capthaCheck.syncByToken())
	{
		qDebug() << "No userverification for token: " << token;
		return nullptr;
	}
	User* newUser = new User;
	newUser->setUsername(capthaCheck.username());
	newUser->setPassword(capthaCheck.password());
	newUser->setEmail(capthaCheck.email());

	VERIFY(capthaCheck.deleteByToken());

	if (!newUser->addUser())
	{
		qCritical() << "User cannot added after captcha check. Something wrong";
		return nullptr;
	}

	return newUser;
}

