#include "textrevisioncontainer.h"
#include <QDebug>
#include "utils/globals.h"
#include "sql/sqlquery.h"
#include <QJsonArray>
#include <QSqlError>

namespace novastory
{

TextRevisionContainer::TextRevisionContainer() : m_synchronized(false)
{
}

bool TextRevisionContainer::sync()
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	JSON_INSERT("text", QString());

	SqlQuery selectQuery;
	selectQuery.prepare("SELECT * FROM textrevisions WHERE userid = :userid");
	selectQuery.bindValue(":userid", userid());

	VERIFY(selectQuery.exec());

	QMap::clear();
	while (selectQuery.next())
	{
		TextRevision revision;
		revision.syncRecord(selectQuery);
		QMap::insert(revision.revisionId(), revision);
	}
	m_synchronized = true;
	qDebug() << "Revisions syncronized for user" << userid();
	return true;
}

int TextRevisionContainer::userid() const
{
	return m_user.userid();
}

TextRevision TextRevisionContainer::insert()
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return TextRevision();
	}

	// Check last revision, may be text the save
	SqlQuery dublicateCheck("SELECT text FROM textrevisions WHERE userid = " + QString::number(m_user.userid()) + " ORDER BY revisionid DESC LIMIT 1");
	Q_ASSERT(dublicateCheck.lastError().type() == QSqlError::NoError);
	if (dublicateCheck.size() == 1)
	{
		VERIFY(dublicateCheck.next());
		if (dublicateCheck.value("text").toString() == m_text) // Dublicate, skip this
		{
			JSON_ERROR("Dublicate of last revision", 3);
			return TextRevision();
		}
	}

	TextRevision revision;
	revision.setUser(m_user);
	revision.setText(m_text);
	revision.setMark(m_mark);
	if (revision.insertSQL())
	{
		Q_ASSERT(revision.revisionId() > 0);
		QMap::insert(revision.revisionId(), revision);
	}
	else
	{
		JSON_ERROR("Something wrong on revision save", 2);
	}
	return revision;
}

TextRevision TextRevisionContainer::insert(const QString& text)
{
	setText(text);
	return insert();
}

novastory::TextRevision TextRevisionContainer::insert(char* text)
{
	return insert(QString(text));
}


novastory::TextRevision TextRevisionContainer::update()
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return TextRevision();
	}

	// Check last revision, may be text the save
	SqlQuery dublicateCheck("SELECT * FROM textrevisions WHERE userid = " + QString::number(m_user.userid()) + " ORDER BY revisionid DESC LIMIT 1");
	Q_ASSERT(dublicateCheck.lastError().type() == QSqlError::NoError);
	if (dublicateCheck.size() == 1)
	{
		VERIFY(dublicateCheck.next());
		if (dublicateCheck.value("text").toString() == m_text) // Dublicate, skip this
		{
			JSON_ERROR("Dublicate of last revision", 3);
			return TextRevision();
		}

		TextRevision rev = value(dublicateCheck.value("revisionid").toInt());
		if (!rev.isValid())
		{
			rev.syncRecord(dublicateCheck);
		}
		rev.setText(m_text);
		if (!m_mark.isEmpty())
		{
			rev.setMark(m_mark);
		}

		Q_ASSERT(rev.isValid());

		if (!rev.isRelease())
		{
			SqlQuery updateQ;
			updateQ.prepare("UPDATE textrevisions SET text = :text, mark = :mark WHERE revisionid = " + dublicateCheck.value("revisionid").toString());
			updateQ.bindValue(":text", rev.text());
			updateQ.bindValue(":mark", rev.mark());

			if (updateQ.exec())
			{
				QMap::insert(rev.revisionId(), rev);
			}
			else
			{
				JSON_ERROR("Something wrong on revision save", 2);
				return TextRevision();
			}
		}
		else
		{
			rev.setRelease(false);
			rev.setRevisionID(-1);
			if (rev.insertSQL())
			{
				Q_ASSERT(rev.revisionId() > 0);
				QMap::insert(rev.revisionId(), rev);
			}
			else
			{
				JSON_ERROR("Something wrong on revision save", 2);
				return TextRevision();
			}
		}

		return rev;
	}
	else
	{
		TextRevision revision;
		revision.setUser(m_user);
		revision.setText(m_text);
		revision.setMark(m_mark);
		if (revision.insertSQL())
		{
			Q_ASSERT(revision.revisionId() > 0);
			QMap::insert(revision.revisionId(), revision);
		}
		else
		{
			JSON_ERROR("Something wrong on revision save", 2);
			return TextRevision();
		}
		return revision;
	}

	return TextRevision();
}

novastory::TextRevision TextRevisionContainer::update(const QString& text)
{
	setText(text);
	return update();
}

novastory::TextRevision TextRevisionContainer::update(char* text)
{
	return update(QString(text));
}


void TextRevisionContainer::setText(const QString& text)
{
	m_text = text;
}

void TextRevisionContainer::setUser(const User& user)
{
	if (m_user != user)
	{
		QMap::clear();
	}
	m_user = user;
}

void TextRevisionContainer::clear()
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return;
	}

	SqlQuery query;
	query.prepare("DELETE FROM textrevisions WHERE userid = ?");
	query.bindValue(0, userid());
	VERIFY(query.exec());
	QMap::clear();
}

bool TextRevisionContainer::release(int targetRevision)
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	if (!m_synchronized)
	{
		sync();
		Q_ASSERT(m_synchronized);
	}

	TextRevision* lastRevision = &last();
	// If last revision promote it to release
	if (lastRevision->revisionId() == targetRevision)
	{
		if (lastRevision->isRelease())
		{
			return true;
		}
		lastRevision->setRelease(true);
		bool status = lastRevision->updateSQL("revisionid", QList<QString>() << "text");
		if (status)
		{
			qDebug() << "Last revision updated to release for user" << userid();
		}
		return status;
	}
	else
	{
		if (!contains(targetRevision))
		{
			JSON_ERROR("Revision not founded", 2);
			return false;
		}
		TextRevision newRevision = value(targetRevision); // copy of old revision
		newRevision.setRelease(true);
		newRevision.setRevisionID(-1);
		VERIFY(newRevision.insertSQL());
		Q_ASSERT(newRevision.revisionId() > 0);
		QMap::insert(newRevision.revisionId(), newRevision);
		qDebug() << "Revision" << targetRevision << "was copy to new release" << newRevision.revisionId() << "revision for user" << userid();
	}

	return true;
}


bool TextRevisionContainer::unrelease(int targetRevision)
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	if (!m_synchronized)
	{
		sync();
		Q_ASSERT(m_synchronized);
	}

	if (!contains(targetRevision))
	{
		JSON_ERROR("Revision not founded", 2);
		return false;
	}

	iterator revisionPoint = find(targetRevision); // copy of old revision
	revisionPoint->setRelease(false);
	bool status = revisionPoint->updateSQL("revisionid", QList<QString>() << "text");
	if (status)
	{
		qDebug() << "Revision" << targetRevision << "was made unreleased now for user" << userid();
	}
	return status;
}

bool TextRevisionContainer::unrelease(const TextRevision& targetRevision)
{
	return unrelease(targetRevision.revisionId());
}


bool TextRevisionContainer::release(const TextRevision& targetRevision)
{
	return release(targetRevision.revisionId());
}

QString TextRevisionContainer::json(bool withoutText /* = false */)
{
	QJsonDocument doc;
	QJsonArray array;
	for (TextRevision& rev : *this)
	{
		array.append(rev.json(withoutText));
	}
	doc.setArray(array);

	return doc.toJson();
}

novastory::TextRevision TextRevisionContainer::revision(int rev)
{
	return value(rev);
}

void TextRevisionContainer::setMark(const QString& text)
{
	m_mark = text;
}

}