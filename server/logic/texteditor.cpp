#include "texteditor.h"
#include <QDebug>
#include "webserver/globals.h"
#include "webserver/sql/sqlquery.h"

namespace novastory
{

TextEditor::TextEditor() : m_lastRevision(0)
{
	setObjectName("texteditor");
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
	updateQuery.prepare("INSERT INTO " + objectName() + "(userid, text, lastrevision) VALUES(:userid, :text, :lastrevision) ON DUPLICATE KEY UPDATE text = :text, lastrevision = :lastrevision");
	updateQuery.bindValue(":userid", userid());
	updateQuery.bindValue(":text", text());
	updateQuery.bindValue(":lastrevision", lastRevision());
	return updateQuery.exec();
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
	if (selectQuery.size() == 1)
	{
		VERIFY(selectQuery.next());
		QString text = selectQuery.value("text").toString();
		setText(text);
		setLastRevision(selectQuery.value("lastrevision").toInt());
		setModifyDate(selectQuery.value("modifydate").toDateTime());

		JSON_INSERT("text", text);
		JSON_INSERT("lastrevision", lastRevision());
		JSON_INSERT("modifydate", modifyDate().toMSecsSinceEpoch());
	}
	return true;
}

const int& TextEditor::lastRevision() const
{
	return m_lastRevision;
}

void TextEditor::setLastRevision(int rev)
{
	m_lastRevision = rev;
}

void TextEditor::resetLastRevision()
{
	m_lastRevision = 0;
}

const QDateTime& TextEditor::modifyDate() const
{
	return m_modify_date;
}

void TextEditor::setModifyDate(const QDateTime& date)
{
	m_modify_date = date;
}

void TextEditor::resetModifyDate()
{
	m_modify_date = QDateTime();
}

}