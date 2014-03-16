#ifndef	TEXTEDITOR_H
#define TEXTEDITOR_H

#include "sql/sqlizable.h"
#include "jsonthrower.h"

class Test_TextEditor;

namespace novastory
{

class TextEditor : protected Sqlizable, public JsonThrower
{
	friend class ::Test_TextEditor;
	Q_OBJECT
	Q_PROPERTY(int userid READ userid WRITE setUserID RESET resetUserid)
	Q_PROPERTY(QString text READ text WRITE setText RESET resetText)
public:
	TextEditor();

	int userid() const;
	void setUserID(int userid);
	void resetUserid();

	const QString& text() const;
	void setText(const QString& text);
	void resetText();

	bool update();
private:
	int m_userid;
	QString m_text;
};

}

#endif
