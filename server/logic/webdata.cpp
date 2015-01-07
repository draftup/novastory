#include "webdata.h"
#include <QDebug>
#include "webserver/globals.h"
#include "config.h"

int novastory::WebData::userid() const
{
	return m_user.userid();
}

void novastory::WebData::setUserid(int userid)
{
	m_user.setUserID(userid);
}

const novastory::WebDataContainer& novastory::WebData::data() const
{
	return m_data;
}

void novastory::WebData::setData(const WebDataContainer& data)
{
	m_data = data;
}

const QString& novastory::WebData::email() const
{
	return m_user.email();
}

void novastory::WebData::setEmail(const QString& email)
{
	m_user.setEmail(email);
}

bool novastory::WebData::sync()
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
	q.prepare("SELECT users.email, " + objectName() + ".* FROM users LEFT JOIN " + objectName() + " ON(" + objectName() + ".userid = users.userid) WHERE " + addwhere);
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
	setData(q.value("data").toByteArray());
	setContentSize(q.value("contentsize").toUInt());
	setContentType(q.value("contenttype").toString());
	setModifyDate(q.value("modifydate").toDateTime());

	Q_ASSERT(contentSize() == q.value("contentsize").toUInt());

	return true;
}

novastory::WebData::WebData()
{
	setObjectName("webdata");
}

bool novastory::WebData::update()
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("User not logined", 2);
		return false;
	}

	int realuserid = userid();

	SqlQuery q;
	q.prepare("INSERT INTO " + objectName() + "(userid, data, contenttype, contentsize) VALUES(:userid, :data, :contenttype, :contentsize) ON DUPLICATE KEY UPDATE data = :data, contenttype = :contenttype, contentsize = :contentsize");
	q.bindValue(":userid", realuserid);
	WebDataContainer b_data = data();
	q.bindValue(":data", b_data);

	if (b_data.size() == 0)
	{
		JSON_ERROR("No avatar data set", 1);
		return false;
	}

	if (b_data.size() > UPLOAD_LIMIT_BYTES)
	{
		JSON_ERROR("WebData is too big", 3);
		return false;
	}

	// We also must control type inserted in db
	b_data.detectMimeType();

	q.bindValue(":contenttype", b_data.mimeType());
	q.bindValue(":contentsize", b_data.size());
	bool status = q.exec();
	if (status)
	{
		qDebug() << "WebData for user " << email() << " updated";
	}
	else
	{
		qDebug() << "WebData for user " << email() << " failed on update";
	}

	return status;
}

bool novastory::WebData::remove()
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("User not logined", 3);
		return false;
	}

	SqlQuery q;
	int quserid = userid();
	Q_ASSERT(quserid > 0);

	q.prepare("DELETE FROM " + objectName() + " WHERE userid = :userid");
	q.bindValue(":userid", quserid);
	bool status = q.exec();
	if (status)
	{
		qDebug() << "WebData for user " << email() << " removed";
	}
	else
	{
		qDebug() << "WebData for user " << email() << " failed on removing";
	}

	return status;
}

const novastory::User& novastory::WebData::user() const
{
	return m_user;
}

void novastory::WebData::setUser(const novastory::User& user)
{
	m_user = user;
}
