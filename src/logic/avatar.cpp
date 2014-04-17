#include "avatar.h"
#include <QDebug>
#include "utils/globals.h"
#include <QMimeDatabase>
#include <QMimeType>

int novastory::Avatar::userid() const
{
	return m_user.userid();
}

void novastory::Avatar::setUserid(int userid)
{
	m_user.setUserID(userid);
}

const QByteArray& novastory::Avatar::avatar() const
{
	return m_avatar;
}

void novastory::Avatar::setAvatar(const QByteArray& avatar)
{
	m_avatar = avatar;
	setContentSize(avatar.size());
}

void novastory::Avatar::setAvatar(const DataImage& image)
{
	setAvatar((QByteArray)image);
	setContentType(image.mimeType());
}

const QString& novastory::Avatar::email() const
{
	return m_user.email();
}

void novastory::Avatar::setEmail(const QString& email)
{
	m_user.setEmail(email);
}

bool novastory::Avatar::sync()
{
	QString addwhere;

	int quserid = userid();
	QString qemail;
	if (quserid > 0)
	{
		addwhere = "users.userid = ?";
	}
	else
	{
		qemail = email();
		if (!qemail.isEmpty())
		{
			addwhere = "users.email = ?";
		}
		else
		{
			return false;
		}
	}

	SqlQuery q;
	q.prepare("SELECT users.email, avatars.* FROM users LEFT JOIN avatars ON(avatars.userid = users.userid) WHERE " + addwhere);
	if (quserid > 0)
	{
		q.bindValue(0, quserid);
	}
	else
	{
		q.bindValue(0, qemail);
	}

	VERIFY(q.exec());

	if (q.size() != 1)
	{
		return false;
	}

	VERIFY(q.next());

	setUserid(q.value("userid").toInt());
	setEmail(q.value("email").toString());
	setAvatar(q.value("avatar").toByteArray());
	setContentSize(q.value("contentsize").toUInt());
	setContentType(q.value("contenttype").toString());

	Q_ASSERT(contentSize() == avatar().size());

	return true;
}

novastory::Avatar::Avatar() : m_conentSize(0)
{
	setObjectName("avatars");
}

bool novastory::Avatar::update()
{
	if(!m_user.isLogined())
	{
		JSON_ERROR("User not logined", 2);
		return false;
	}

	int realuserid = userid();

	SqlQuery q;
	q.prepare("INSERT INTO " + objectName() + "(userid, avatar, contenttype, contentsize) VALUES(:userid, :avatar, :contenttype, :contentsize) ON DUPLICATE KEY UPDATE avatar = :avatar, contenttype = :contenttype, contentsize = :contentsize");
	q.bindValue(":userid", realuserid);
	QByteArray ava = avatar();
	q.bindValue(":avatar", ava);

	if (ava.size() == 0)
	{
		JSON_ERROR("No avatar data set", 1);
		return false;
	}

	QMimeDatabase db;
	QMimeType mime = db.mimeTypeForData(ava);

	q.bindValue(":contenttype", mime.name());
	q.bindValue(":contentsize", ava.size());
	return q.exec();
}

bool novastory::Avatar::remove()
{
	if(!m_user.isLogined())
	{
		JSON_ERROR("User not logined", 3);
		return false;
	}

	SqlQuery q;
	int quserid = userid();
	Q_ASSERT(quserid > 0);

	q.prepare("DELETE FROM " + objectName() + " WHERE userid = :userid");
	q.bindValue(":userid", quserid);
	return q.exec();
}

const novastory::User& novastory::Avatar::user() const
{
	return m_user;
}

void novastory::Avatar::setUser( const novastory::User& user )
{
	m_user = user;
}
