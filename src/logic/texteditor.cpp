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
	return m_user.userid();
}

void TextEditor::setUserID(int userid)
{
	m_user.setUserID(userid);
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
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	if (m_text.isNull())
	{
		SqlQuery removeQuery;
		removeQuery.prepare("DELETE FROM " + objectName() + " WHERE userid = ?");
		removeQuery.bindValue(0, userid());
		return removeQuery.exec();
	}

	SqlQuery updateQuery;
	updateQuery.prepare("INSERT INTO " + objectName() + "(userid, text) VALUES(:userid, :text) ON DUPLICATE KEY UPDATE text = :text");
	updateQuery.bindValue(":userid", userid());
	updateQuery.bindValue(":text", text());
	return updateQuery.exec();
}

void TextEditor::setUser( const User& user )
{
	m_user = user;
}

bool TextEditor::sync()
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	JSON_INSERT("text", QString());

	SqlQuery selectQuery;
	selectQuery.prepare("SELECT * FROM " + objectName() + " WHERE userid = :userid");
	selectQuery.bindValue(":userid", userid());

	VERIFY(selectQuery.exec());
	if(selectQuery.size() == 1)
	{
		VERIFY(selectQuery.next());
		QString text = selectQuery.value("text").toString();
		setText(text);

		JSON_INSERT("text", text);
	}
	return true;
}

}