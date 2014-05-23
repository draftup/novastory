#ifndef	TEXTREVISION_H
#define TEXTREVISION_H

#include "sql/sqlizable.h"
#include "jsonthrower.h"
#include "user.h"

class Test_TextRevision;

namespace novastory
{

class TextRevision : protected Sqlizable, public JsonThrower
{
	friend class ::Test_TextRevision;
	Q_OBJECT
	Q_PROPERTY(int revisionid READ revisionId WRITE setRevisionID)
	Q_PROPERTY(int userid READ userid WRITE setUserID)
	Q_PROPERTY(int revision READ revision WRITE setRevision)
	Q_PROPERTY(QString text READ text WRITE setText RESET resetText)
public:
	TextRevision();

	int userid() const;
	void setUserID(int userid);
	void setUser(const User& user);

	const QString& text() const;
	void setText(const QString& text);
	void resetText();

	const int& revision() const;
	void setRevision(int revision);

	const int& revisionId() const;
	void setRevisionID(int revision);
private:
	User m_user;
	QString m_text;
	int m_revision;
	int m_revisionId;
};

}

#endif
