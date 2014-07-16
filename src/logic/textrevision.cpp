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

	return revision;
}

bool TextRevision::isValid() const
{
	return m_revisionId > 0;
}

}