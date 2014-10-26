#ifndef	TEXTEDITOR_H
#define TEXTEDITOR_H

#include "sql/sqlizable.h"
#include "jsonthrower.h"
#include "user.h"

class Test_TextEditor;

namespace novastory
{

class TextEditor : protected Sqlizable, public JsonThrower
{
	friend class ::Test_TextEditor;
	Q_OBJECT
	Q_PROPERTY(int userid READ userid WRITE setUserID)
	Q_PROPERTY(QString text READ text WRITE setText RESET resetText)
	Q_PROPERTY(int lastrevision READ lastRevision WRITE setLastRevision RESET resetLastRevision)
	Q_PROPERTY(QDateTime modifydate READ modifyDate WRITE setModifyDate RESET resetModifyDate)
public:
	TextEditor();

	int userid() const;
	void setUserID(int userid);
	void setUser(const User& user);

	const QString& text() const;
	void setText(const QString& text);
	void resetText();

	const int& lastRevision() const;
	void setLastRevision(int rev);
	void resetLastRevision();

	const QDateTime& modifyDate() const;
	void setModifyDate(const QDateTime& date);
	void resetModifyDate();

	bool update();
	bool sync();
private:
	User m_user;
	QString m_text;
	int m_lastRevision;
	QDateTime m_modify_date;
};

}

#endif
