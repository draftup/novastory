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
}