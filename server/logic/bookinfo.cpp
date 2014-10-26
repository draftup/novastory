#include "bookinfo.h"
#include <QDebug>
#include "utils/globals.h"
#include "sql/sqlquery.h"

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


}