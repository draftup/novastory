#include "textrevision.h"
#include <QDebug>
#include "utils/globals.h"
#include "sql/sqlquery.h"

namespace novastory
{

int TextRevision::userid() const
{
	return m_user.userid();
}

void TextRevision::setUserID(int userid)
{
	m_user.setUserID(userid);
}

const QString& TextRevision::text() const
{
	return m_text;
}

void TextRevision::setText(const QString& text)
{
	m_text = text;
}

void TextRevision::resetText()
{
	m_text = QString();
}

const QString& TextRevision::mark() const
{
	return m_mark;
}

void TextRevision::setMark(const QString& mark)
{
	m_mark = mark;
}

void TextRevision::resetMark()
{
	m_mark = QString();
}

void TextRevision::setUser(const User& user)
{
	m_user = user;
}

const int& TextRevision::revisionId() const
{
	return m_revisionId;
}

void TextRevision::setRevisionID(int revision)
{
	m_revisionId = revision;
}

TextRevision::TextRevision() : m_release(false), m_revisionId(-1)
{
	setObjectName("textrevisions");
	setProperty("auto_increment", QVariant("revisionid"));
	setProperty("primary_key", QVariant("revisionid"));
}

bool TextRevision::isRelease() const
{
	return m_release;
}

void TextRevision::setRelease(bool rel)
{
	m_release = rel;
}

void TextRevision::resetRelease()
{
	m_release = false;
}

bool TextRevision::operator==(const TextRevision& rv) const
{
	return revisionId() == rv.revisionId();
}

QJsonObject TextRevision::json(bool withoutText /* = false */) const
{
	QJsonObject revision;
	revision.insert("revisionid", revisionId());
	revision.insert("userid", userid());
	revision.insert("isRelease", isRelease());
	if (!withoutText)
	{
		revision.insert("text", text());
	}
	revision.insert("textLength", text().length());
	revision.insert("createDate", createDate().toMSecsSinceEpoch());
	revision.insert("modifyDate", modifyDate().toMSecsSinceEpoch());
	revision.insert("mark", mark());

	return revision;
}

bool TextRevision::isValid() const
{
	return m_revisionId > 0;
}

const QDateTime& TextRevision::createDate() const
{
	return m_create_date;
}

void TextRevision::setCreateDate(const QDateTime& date)
{
	m_create_date = date;
}

void TextRevision::resetCreateDate()
{
	m_create_date = QDateTime();
}

const QDateTime& TextRevision::modifyDate() const
{
	return m_modify_date;
}

void TextRevision::setModifyDate(const QDateTime& date)
{
	m_modify_date = date;
}

void TextRevision::resetModifyDate()
{
	m_modify_date = QDateTime();
}

}