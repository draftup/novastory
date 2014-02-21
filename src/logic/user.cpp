#include "user.h"
#include <QVariant>
#include <QDebug>
#include <QSqlRecord>
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

	setPassword(generatePassword(sha1(password), currentSalt));
}

bool novastory::User::addUser()
{
	if (m_password.isEmpty() || m_salt.isEmpty() || m_email.isEmpty())
	{
		return false;    // something is empty
	}

	// Checking if user already exist
	SqlQuery query;
	query.prepare("SELECT userid FROM users WHERE email = ?");
	query.bindValue(0, m_email);
	VERIFY(query.exec());

	if (query.size() != 0)
	{
		qDebug() << "User" << m_email << "already in database";
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
	newUser->setPassword(capthaCheck.password());
	newUser->setSalt(capthaCheck.salt());
	newUser->setEmail(capthaCheck.email());

	VERIFY(capthaCheck.deleteByToken());

	if (!newUser->addUser())
	{
		qCritical() << "User cannot added after captcha check. Something wrong";
		return nullptr;
	}

	return newUser;
}

void novastory::User::setSHA1Password(const QString& sha1password)
{
	QString currentSalt = salt();

	if (currentSalt.isEmpty())
	{
		// if old salt is empty we must generate new salt
		currentSalt = generateSalt();
		setSalt(currentSalt);
	}

	setPassword(generatePassword(sha1password, currentSalt));
}

bool novastory::User::login(const QString& semail, const QString& sha1password)
{
	if (semail.isEmpty() || sha1password.isEmpty())
	{
		JSON_ERROR("Email or password are not valid", 1);
		return false;
	}

	setEmail(semail);

	SqlQuery query;
	query.prepare("SELECT * FROM users WHERE email = :email");
	query.bindValue(":email", email());

	VERIFY(query.exec());

	if (query.size() != 1)
	{
		JSON_ERROR("User with such email not founded: " + email(), 2);
		return false;
	}

	VERIFY(query.next());

	int saltNo = query.record().indexOf("salt");
	QString qsalt = query.value(saltNo).toString();
	int passwordNo = query.record().indexOf("password");
	QString qpassword = query.value(passwordNo).toString();
	int useridNo = query.record().indexOf("userid");
	int quserid = query.value(useridNo).toInt();

	if (qpassword != generatePassword(sha1password, qsalt))
	{
		JSON_ERROR("Password for user " + semail + " was wrong", 3);
		return false;
	}

	Q_ASSERT(quserid > 0);

	qint64 current_time = unixtime();
	m_token = current_time + QString("-") + generateToken(current_time, quserid, qsalt);

	JSON_INSERT("logined", true);
	JSON_INSERT("token", m_token);

	query.seek(-1);
	return syncProcess(query);
}

bool novastory::User::isLogined() const
{
	return !m_token.isEmpty();
}

const QString& novastory::User::token()
{
	return m_token;
}

