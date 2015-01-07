#include "bookinfo.h"
#include <QDebug>
#include "webserver/globals.h"
#include "webserver/sql/sqlquery.h"

namespace novastory
{

BookInfo::BookInfo() : m_bookid(-1), m_revisionid(-1)
{
	setObjectName("books");
	setProperty("auto_increment", QVariant("bookid"));
	setProperty("primary_key", QVariant("bookid"));
}

int BookInfo::bookid() const
{
	return m_bookid;
}

void BookInfo::setBookID(int bookid)
{
	m_bookid = bookid;
}

const QString& BookInfo::description() const
{
	return m_description;
}

void BookInfo::setDescription(const QString& text)
{
	m_description = text;
}

void BookInfo::resetDescription()
{
	m_description = QString();
}

int BookInfo::textid() const
{
	return m_revisionid;
}

void BookInfo::setTextID(int textid)
{
	m_revisionid = textid;
}

void BookInfo::resetTextID()
{
	m_revisionid = -1;
}

bool BookInfo::sync()
{
	if (bookid() <= 0)
	{
		JSON_ERROR("book id null", 1);
		return false;
	}

	return syncSQL("bookid");
}

bool BookInfo::update()
{
	if (!isLogged())
	{
		JSON_ERROR("not loggined", 1);
		return false;
	}

	return insertSQL(true, true);
}

bool BookInfo::remove()
{
	if (!isLogged())
	{
		JSON_ERROR("not loggined", 1);
		return false;
	}

	return removeSQL("bookid");
}


}