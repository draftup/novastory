#include "texteditor.h"
#include <QDebug>
#include "utils/globals.h"
#include "sql/sqlquery.h"

namespace novastory
{

TextEditor::TextEditor()
{
	setObjectName("texteditor");
}

int TextEditor::userid() const
{
	return m_userid;
}

void TextEditor::setUserID(int userid)
{
	m_userid = userid;
}

void TextEditor::resetUserid()
{
	m_userid = -1;
}

const QString& TextEditor::text() const
{
	return m_text;
}

void TextEditor::setText(const QString& text)
{
	m_text = text;
}

void TextEditor::resetText()
{
	m_text = QString();
}

bool TextEditor::update()
{
	if(m_userid < 1)
	{
		JSON_ERROR("Email not valid", 1);
		return false;
	}

	SqlQuery userExistCheck;
	userExistCheck.prepare("SELECT userid FROM users WHERE userid = ?");
	userExistCheck.bindValue(0, m_userid);
	VERIFY(userExistCheck.exec());
	if(userExistCheck.size() < 1)
	{
		JSON_ERROR("No such user", 2);
		return false;
	}

	if(m_text.isNull())
	{
		SqlQuery removeQuery;
		removeQuery.prepare("DELETE FROM " + objectName() + " WHERE userid = ?");
		removeQuery.bindValue(0, m_userid);
		return removeQuery.exec();
	}

	SqlQuery updateQuery;
	updateQuery.prepare("INSERT INTO " + objectName() + "(userid, text) VALUES(:userid, :text) ON DUPLICATE KEY UPDATE text = :text");
	updateQuery.bindValue(":userid", m_userid);
	updateQuery.bindValue(":text", m_text);
	return updateQuery.exec();
}

}