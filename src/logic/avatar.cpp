#include "avatar.h"
#include <QDebug>
#include "utils/globals.h"

int novastory::Avatar::userid() const
{
	return m_userid;
}

void novastory::Avatar::setUserid( int userid )
{
	m_userid = userid;
}

const QByteArray& novastory::Avatar::avatar() const
{
	return m_avatar;
}

void novastory::Avatar::setAvatar( const QByteArray& avatar )
{
	m_avatar = avatar;
}

const QString& novastory::Avatar::email() const
{
	return m_email;
}

void novastory::Avatar::setEmail( const QString& email )
{
	m_email = email;
}

bool novastory::Avatar::sync()
{
	QString addwhere;

	int quserid = userid();
	QString qemail;
	if(quserid > 0)
	{
		addwhere = "users.userid = ?";
	}
	else
	{
		qemail = email();
		if(!qemail.isEmpty())
		{
			addwhere = "users.email = ?";
		}
		else
		{
			return false;
		}
	}

	SqlQuery q;
	q.prepare("SELECT users.email, users.userid, avatars.avatar FROM users LEFT JOIN avatars ON(avatars.userid = users.userid) WHERE " + addwhere);
	if(quserid > 0)
		q.bindValue(0, quserid);
	else
		q.bindValue(0, qemail);

	VERIFY(q.exec());

	if(q.size() != 1)
	{
		return false;
	}

	VERIFY(q.next());

	setUserid(q.value("userid").toInt());
	setEmail(q.value("email").toString());
	setAvatar(q.value("avatar").toByteArray());

	return true;
}

novastory::Avatar::Avatar()
{
	setObjectName("avatars");
}

bool novastory::Avatar::update()
{
	SqlQuery q;
	q.prepare("INSERT INTO " + objectName() + "(userid, avatar) VALUES(:userid, :avatar) ON DUPLICATE KEY UPDATE avatar = :avatar");
	q.bindValue(":userid", userid());
	q.bindValue(":avatar", avatar());
	return q.exec();
}
