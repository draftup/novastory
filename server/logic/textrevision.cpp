#include "textrevision.h"
#include <QDebug>
#include <QJsonArray>
#include "webserver/globals.h"
#include "webserver/sql/sqlquery.h"

namespace novastory
{

const QString& TextRevision::text() const
{
	return m_text;
}

void TextRevision::setText(const QString& text)
{
	m_text = text;
}

void TextRevision::resetText()
{
	m_text = QString();
}

const QString& TextRevision::mark() const
{
	return m_mark;
}

void TextRevision::setMark(const QString& mark)
{
	m_mark = mark;
}

void TextRevision::resetMark()
{
	m_mark = QString();
}

const int& TextRevision::revisionId() const
{
	return m_revisionId;
}

void TextRevision::setRevisionID(int revision)
{
	m_revisionId = revision;
}

TextRevision::TextRevision() : m_release(0), m_revisionId(-1), m_parentId(0), m_type("OTHER")
{
	setObjectName("textrevisions");
	setProperty("auto_increment", QVariant("revisionid"));
	setProperty("primary_key", QVariant("revisionid"));
}

bool TextRevision::isRelease() const
{
	return m_release > 0;
}

void TextRevision::setRelease(int rel)
{
	m_release = rel;
}

void TextRevision::setRelease(bool rel)
{
	m_release = (int)rel;
}

void TextRevision::resetRelease()
{
	m_release = 0;
}

bool TextRevision::operator==(const TextRevision& rv) const
{
	return revisionId() == rv.revisionId();
}

QJsonObject TextRevision::json(bool withoutText /* = false */) const
{
	QJsonObject revision;
	revision.insert("revisionid", revisionId());
	revision.insert("userid", userid());
	revision.insert("isRelease", isRelease());
	if (!withoutText)
	{
		revision.insert("text", text());
	}
	revision.insert("textLength", text().length());
	revision.insert("createDate", createDate().toMSecsSinceEpoch());
	revision.insert("modifyDate", modifyDate().toMSecsSinceEpoch());
	revision.insert("parentid", parent());
	revision.insert("mark", mark());
	revision.insert("type", type());

	QJsonArray childs;
	for (const TextRevision& rev : m_childs)
	{
		childs.append(rev.json());
	}
	revision.insert("childs", childs);

	return revision;
}

bool TextRevision::isValid() const
{
	return m_revisionId > 0;
}

const QDateTime& TextRevision::createDate() const
{
	return m_create_date;
}

void TextRevision::setCreateDate(const QDateTime& date)
{
	m_create_date = date;
}

void TextRevision::resetCreateDate()
{
	m_create_date = QDateTime();
}

const QDateTime& TextRevision::modifyDate() const
{
	return m_modify_date;
}

void TextRevision::setModifyDate(const QDateTime& date)
{
	m_modify_date = date;
}

void TextRevision::resetModifyDate()
{
	m_modify_date = QDateTime();
}

const int& TextRevision::parent() const
{
	return m_parentId;
}

void TextRevision::setParent(int id)
{
	m_parentId = id;
}

void TextRevision::resetParent()
{
	m_parentId = -1;
}

void TextRevision::appendChild(const TextRevision& rev)
{
	m_childs.append(rev);
}

void TextRevision::clearChilds()
{
	m_childs.clear();
}

void TextRevision::resetType()
{
	m_type = "OTHER";
}

void TextRevision::setType(QString id)
{
	m_type = id;
}

const QString& TextRevision::type() const
{
	return m_type;
}

int TextRevision::releaseValue() const
{
	return m_release;
}

}