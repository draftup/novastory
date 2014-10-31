#ifndef	BOOKINFO_H
#define BOOKINFO_H

#include "sql/sqlizable.h"
#include "jsonthrower.h"
#include "logic/loggedactions.h"

class Test_BookInfo;

namespace novastory
{

class BookInfo : protected Sqlizable, public JsonThrower, public LoggedActions
{
	friend class ::Test_BookInfo;
	Q_OBJECT
	Q_PROPERTY(int bookid READ bookid WRITE setBookID)
	Q_PROPERTY(int textid READ textid WRITE setTextID RESET resetTextID)
	Q_PROPERTY(QString description READ description WRITE setDescription RESET resetDescription)
public:
	BookInfo();

	int bookid() const;
	void setBookID(int bookid);

	const QString& description() const;
	void setDescription(const QString& text);
	void resetDescription();

	int textid() const;
	void setTextID(int textid);
	void resetTextID();

	bool sync();
	bool update();
private:
	int m_bookid;
	int m_revisionid;
	QString m_description;
};

}

#endif
