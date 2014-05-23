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

	const int& TextRevision::revision() const
	{
		return m_revision;
	}

	void TextRevision::setRevision( int revision )
	{
		m_revision = revision;
	}

	const int& TextRevision::revisionId() const
	{
		return m_revisionId;
	}

	void TextRevision::setRevisionID( int revision )
	{
		m_revisionId = revision;
	}

}