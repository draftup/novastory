#include "users.h"
#include <QVariant>

int novastory::Users::userid() const
{
	return m_userid;
}

void novastory::Users::setUserID( int userid )
{
	m_userid = userid;
}

const QString& novastory::Users::password() const
{
	return m_password;
}

void novastory::Users::setPassword( const QString& password )
{
	m_password = password;
}

const QString& novastory::Users::salt() const
{
	return m_salt;
}

void novastory::Users::setSalt( const QString& salt )
{
	m_salt = salt;
}

const QString& novastory::Users::email() const
{
	return m_email;
}

void novastory::Users::setEmail( const QString& email )
{
	m_email = email;
}

const QString& novastory::Users::username() const
{
	return m_username;
}

void novastory::Users::setUsername( const QString& username )
{
	m_username = username;
}

novastory::Users::Users() : m_userid(0)
{
	setObjectName("users");
	setProperty("auto_increment", QVariant("userid"));
}
