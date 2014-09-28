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
	m_table_name = "textrevisions";
	m_left_name = "left_key";
	m_right_name = "right_key";
	m_data_name = "text";
	m_id_name = "revisionid";
	m_parent_name = "parent_id";
}

bool TextRevisionContainer::sync(int parentId /* = 0 */)
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	JSON_INSERT("text", QString());

	SqlQuery selectQuery;
	selectQuery.prepare("SELECT * FROM textrevisions WHERE right_key - left_key = 1 AND userid = :userid" + ((parentId > 0) ? " AND " + m_parent_name + " = :parentid" : ""));
	selectQuery.bindValue(":userid", userid());
	if (parentId > 0)
	{
		selectQuery.bindValue(":parentid", parentId);
	}

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

TextRevision TextRevisionContainer::insert(int parentId /* = 0 */)
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return TextRevision();
	}

	// Check last revision, may be text the save
	if (parentId > 0)
	{
		SqlQuery dublicateCheck("SELECT text FROM textrevisions WHERE userid = " + QString::number(m_user.userid()) + " AND " + m_parent_name + " = " + QString::number(parentId) + " ORDER BY revisionid DESC LIMIT 1");
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
	}

	TextRevision revision;
	QHash<QString, QVariant> valuesToDB;
	revision.setUser(m_user);
	valuesToDB.insert("userid", m_user.userid());
	revision.setText(m_text);
	valuesToDB.insert("text", m_text);
	revision.setMark(m_mark);
	valuesToDB.insert("mark", m_mark);
	m_where_coincidence = "userid = " + QString::number(m_user.userid());
	Q_ASSERT(parentId >= 0);
	int revisionid = -1;
	// If no parent we must create some folder
	if (parentId > 0)
	{
		revisionid = NestedSet::insert(parentId, valuesToDB);
	}
	else
	{
		// no parent create folder then add text to this folder
		QHash<QString, QVariant> folderInfo;
		folderInfo.insert("userid", m_user.userid());
		folderInfo.insert("mark", "New Text");
		int folderid = NestedSet::insert(0, folderInfo);
		Q_ASSERT(folderid > 0);
		revisionid = NestedSet::insert(folderid, valuesToDB);
	}

	if (revisionid > 0)
	{
		revision.setRevisionID(revisionid);
		Q_ASSERT(revision.revisionId() > 0);
		QMap::insert(revision.revisionId(), revision);
	}
	else
	{
		JSON_ERROR("Something wrong on revision save", 2);
	}
	return revision;
}

TextRevision TextRevisionContainer::insert(const QString& text, int parentId /* = 0 */)
{
	setText(text);
	return insert(parentId);
}

novastory::TextRevision TextRevisionContainer::insert(char* text, int parentId /* = 0 */)
{
	return insert(QString(text), parentId);
}


novastory::TextRevision TextRevisionContainer::update(int revision /* = 0 */)
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return TextRevision();
	}

	// Check last revision, may be text the save
	SqlQuery dublicateCheck("SELECT * FROM textrevisions WHERE userid = " + QString::number(m_user.userid()) + ((revision > 0) ? " AND revisionid = " + QString::number(revision) : "") + " ORDER BY revisionid DESC LIMIT 1");
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

			QHash<QString, QVariant> valuesToDB;
			valuesToDB.insert("userid", rev.userid());
			valuesToDB.insert("text", rev.text());
			valuesToDB.insert("mark", rev.mark());
			valuesToDB.insert("release", rev.isRelease());
			m_where_coincidence = "userid = " + QString::number(rev.userid());
			int revisionid = NestedSet::insert(rev.parent(), valuesToDB);
			if (revisionid > 0)
			{
				rev.setRevisionID(revisionid);
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
		QHash<QString, QVariant> valuesToDB;
		valuesToDB.insert("userid", m_user.userid());
		valuesToDB.insert("text", m_text);
		valuesToDB.insert("mark", m_mark);
		m_where_coincidence = "userid = " + QString::number(m_user.userid());

		// we also need to create folder
		QHash<QString, QVariant> folderInfo;
		folderInfo.insert("userid", m_user.userid());
		folderInfo.insert("mark", "New Text");
		int folderid = NestedSet::insert(0, folderInfo);
		Q_ASSERT(folderid > 0);

		int revisionid = NestedSet::insert(folderid, valuesToDB);
		if (revisionid > 0)
		{
			revision.setRevisionID(revisionid);
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

novastory::TextRevision TextRevisionContainer::update(const TextRevision& revision, const QString& text /* = QString()*/)
{
	setText(text);
	return update(revision.revisionId());
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
		QHash<QString, QVariant> valuesToDB;
		valuesToDB.insert("userid", newRevision.userid());
		valuesToDB.insert("text", newRevision.text());
		valuesToDB.insert("mark", newRevision.mark());
		valuesToDB.insert("release", newRevision.isRelease());
		m_where_coincidence = "userid = " + QString::number(newRevision.userid());
		int revisionid = NestedSet::insert(0, valuesToDB);
		Q_ASSERT(revisionid > 0);
		newRevision.setRevisionID(revisionid);
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

QString TextRevisionContainer::treeFolders()
{
	QJsonDocument doc;
	m_where_coincidence = "userid = " + QString::number(m_user.userid());
	SqlQuery tree = NestedSet::notLeefs();

	QJsonArray array;
	QList<TextRevision> revisions;
	int lastid = -1;
	while (tree.next())
	{
		TextRevision revision;
		revision.syncRecord(tree);
		if (lastid > 0 && lastid == revision.parent())
		{
			TextRevision& rev = revisions.last();
			if (rev.revisionId() != lastid)
			{
				qCritical() << "Wrong treefolder structure";
			}
			Q_ASSERT(rev.revisionId() == lastid);
			rev.appendChild(revision);
		}
		else
		{
			revisions.append(revision);
		}

		lastid = revision.revisionId();
	}
	for (const TextRevision& rev : revisions)
	{
		array.append(rev.json());
	}
	doc.setArray(array);

	return doc.toJson();
}

novastory::TextRevision TextRevisionContainer::revision(int rev)
{
	if (contains(rev))
	{
		return value(rev);
	}
	else
	{
		QString userWhere;
		if (userid() > 0)
		{
			userWhere = "AND userid = :userid";
		}
		else
		{
			userWhere = "AND `release` = 1";
		}
		SqlQuery selectQuery;
		selectQuery.prepare("SELECT * FROM textrevisions WHERE revisionid = :revid " + userWhere + " LIMIT 1");
		if (userid() > 0)
		{
			selectQuery.bindValue(":userid", userid());
		}
		selectQuery.bindValue(":revid", rev);
		VERIFY(selectQuery.exec());
		if (selectQuery.size() != 1)
		{
			return TextRevision();
		}
		VERIFY(selectQuery.next());
		TextRevision revision;
		revision.syncRecord(selectQuery);
		QMap::insert(revision.revisionId(), revision);
		return revision;
	}
}

void TextRevisionContainer::setMark(const QString& text)
{
	m_mark = text;
}

bool TextRevisionContainer::removeRevision(const TextRevision& targetRevision)
{
	return removeRevision(targetRevision.revisionId());
}

bool TextRevisionContainer::removeRevision(int revision)
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	m_where_coincidence = "userid = " + QString::number(m_user.userid());
	if (NestedSet::remove(revision))
	{
		QMap::remove(revision);
		return true;
	}

	return false;
}

bool TextRevisionContainer::updateMark(int revision)
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	SqlQuery updateQ;
	updateQ.prepare("UPDATE textrevisions SET mark = :mark WHERE revisionid = " + QString::number(revision));
	updateQ.bindValue(":mark", m_mark);
	if (updateQ.exec())
	{
		QMap<int, TextRevision>::iterator it = QMap::find(revision);
		if (it != end())
		{
			it->setMark(m_mark);
		}
		return true;
	}
	else
	{
		JSON_ERROR("Error", 2);
		return false;
	}

	return false;
}

bool TextRevisionContainer::updateMark(const TextRevision& targetRevision)
{
	return updateMark(targetRevision.revisionId());
}

}