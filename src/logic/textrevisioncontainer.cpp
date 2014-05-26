#include "textrevisioncontainer.h"
#include <QDebug>
#include "utils/globals.h"
#include "sql/sqlquery.h"

namespace novastory
{

TextRevisionContainer::TextRevisionContainer()
{
}

bool TextRevisionContainer::sync()
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	JSON_INSERT("text", QString());

	SqlQuery selectQuery;
	selectQuery.prepare("SELECT * FROM textrevisions WHERE userid = :userid");
	selectQuery.bindValue(":userid", userid());

	VERIFY(selectQuery.exec());

	QMap::clear();
	while (selectQuery.next())
	{
		TextRevision revision;
		revision.syncRecord(selectQuery);
		insert(revision.revisionId(), revision);
	}

	return true;
}

int TextRevisionContainer::userid() const
{
	return m_user.userid();
}

bool TextRevisionContainer::save()
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	TextRevision revision;
	revision.setUser(m_user);
	revision.setText(m_text);
	bool status = revision.insertSQL();
	if (status)
	{
		Q_ASSERT(revision.revisionId() > 0);
		insert(revision.revisionId(), revision);
	}
	return status;
}

bool TextRevisionContainer::save(const QString& text)
{
	setText(text);
	return save();
}

void TextRevisionContainer::setText(const QString& text)
{
	m_text = text;
}

void TextRevisionContainer::setUser(const User& user)
{
	if (m_user != user)
	{
		QMap::clear();
	}
	m_user = user;
}

void TextRevisionContainer::clear()
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return;
	}

	SqlQuery query;
	query.prepare("DELETE FROM textrevisions WHERE userid = ?");
	query.bindValue(0, userid());
	VERIFY(query.exec());
	QMap::clear();
}

}