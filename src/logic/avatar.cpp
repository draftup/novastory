#include "avatar.h"
#include <QDebug>
#include "utils/globals.h"
#include <QMimeDatabase>
#include <QMimeType>

int novastory::Avatar::userid() const
{
	return m_userid;
}

void novastory::Avatar::setUserid(int userid)
{
	m_userid = userid;
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

void novastory::Avatar::setAvatar( const DataImage& image )
{
	setAvatar((QByteArray)image);
	setContentType(image.mimeType());
}

const QString& novastory::Avatar::email() const
{
	return m_email;
}

void novastory::Avatar::setEmail(const QString& email)
{
	m_email = email;
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

novastory::Avatar::Avatar() : m_userid(-1), m_conentSize(0)
{
	setObjectName("avatars");
}

bool novastory::Avatar::update()
{
	SqlQuery q;
	q.prepare("INSERT INTO " + objectName() + "(userid, avatar, contenttype, contentsize) VALUES(:userid, :avatar, :contenttype, :contentsize) ON DUPLICATE KEY UPDATE avatar = :avatar, contenttype = :contenttype, contentsize = :contentsize");
	q.bindValue(":userid", userid());
	QByteArray ava = avatar();
	q.bindValue(":avatar", ava);

	if(ava.size() == 0)
	{
		JSON_ERROR("No avatar data set", 1);
		return false;
	}

	qDebug() << ava.size();

	QMimeDatabase db;
	QMimeType mime = db.mimeTypeForData(ava);

	q.bindValue(":contenttype", mime.name());
	q.bindValue(":contentsize", ava.size());
	return q.exec();
}

bool novastory::Avatar::remove()
{
	SqlQuery q;
	int quserid = userid();
	if (quserid > 0)
	{
		q.prepare("DELETE FROM " + objectName() + " WHERE userid = :userid");
		q.bindValue(":userid", quserid);
		return q.exec();
	}

	QString qemail = email();
	if (qemail.isEmpty())
	{
		return false;
	}

	q.prepare("DELETE avatars FROM avatars INNER JOIN users ON(users.userid = avatars.userid) WHERE users.email = :email");
	q.bindValue(":email", qemail);

	return q.exec();
}
