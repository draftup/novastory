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
	selectQuery.prepare("SELECT * FROM textrevisions WHERE `type` = 'REVISION' AND userid = :userid" + ((parentId > 0) ? " AND " + m_parent_name + " = :parentid" : ""));
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
	revision.setType("REVISION");
	valuesToDB.insert("type", "REVISION");
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
		folderInfo.insert("type", "TEXT");
		int folderid = NestedSet::insert(0, folderInfo);
		Q_ASSERT(folderid > 0);
		revisionid = NestedSet::insert(folderid, valuesToDB);
		revision.setParent(folderid);
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


novastory::TextRevision TextRevisionContainer::newProject(const QString& projectName, int parentId /*= 0*/)
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return TextRevision();
	}

	TextRevision revision;
	QHash<QString, QVariant> valuesToDB;
	revision.setUser(m_user);
	valuesToDB.insert("userid", m_user.userid());
	setMark(projectName);
	revision.setMark(m_mark);
	valuesToDB.insert("mark", m_mark);
	revision.setType("OTHER");
	valuesToDB.insert("type", "OTHER");
	m_where_coincidence = "userid = " + QString::number(m_user.userid());
	int revisionid = -1;
	// If no parent we must create some folder
	if (parentId > 0)
	{
		revisionid = NestedSet::insert(parentId, valuesToDB);
	}
	else
	{
		revisionid = NestedSet::insert(0, valuesToDB);
	}

	if (revisionid > 0)
	{
		revision.setRevisionID(revisionid);
		Q_ASSERT(revision.revisionId() > 0);
		//QMap::insert(revision.revisionId(), revision);
	}
	else
	{
		JSON_ERROR("Something wrong on revision save", 2);
	}

	return revision;
}


novastory::TextRevision TextRevisionContainer::update(int revision /* = 0 */)
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return TextRevision();
	}

	// Check last revision, may be text the save
	SqlQuery dublicateCheck("SELECT * FROM textrevisions WHERE userid = " + QString::number(m_user.userid()) + ((revision > 0) ? " AND revisionid = " + QString::number(revision) : "") + " AND `type` = 'REVISION' ORDER BY revisionid DESC LIMIT 1");
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
			valuesToDB.insert("type", rev.type());
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
		revision.setType("REVISION");
		QHash<QString, QVariant> valuesToDB;
		valuesToDB.insert("userid", m_user.userid());
		valuesToDB.insert("text", m_text);
		valuesToDB.insert("mark", m_mark);
		valuesToDB.insert("type", "REVISION");
		m_where_coincidence = "userid = " + QString::number(m_user.userid());

		// we also need to create folder
		QHash<QString, QVariant> folderInfo;
		folderInfo.insert("userid", m_user.userid());
		folderInfo.insert("mark", "New Text");
		folderInfo.insert("type", "TEXT");
		int folderid = NestedSet::insert(0, folderInfo);
		Q_ASSERT(folderid > 0);

		int revisionid = NestedSet::insert(folderid, valuesToDB);
		if (revisionid > 0)
		{
			revision.setRevisionID(revisionid);
			Q_ASSERT(revision.revisionId() > 0);
			revision.setParent(folderid);
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

	SqlQuery releaseRevInfo("SELECT left_key,right_key,`type`,`release` FROM textrevisions WHERE revisionid = " + QString::number(targetRevision));
	if (releaseRevInfo.size() != 1)
	{
		JSON_ERROR("Revision not founded", 2);
		return false;
	}
	VERIFY(releaseRevInfo.next());

	QString left_key = releaseRevInfo.value("left_key").toString();
	QString right_key = releaseRevInfo.value("right_key").toString();
	QString relType = releaseRevInfo.value("type").toString();
	int releaseVal = releaseRevInfo.value("release").toInt();

	if (releaseVal != 0)
	{
		JSON_ERROR("Already released", 3);
		return false;
	}

	if (relType != "REVISION")
	{
		JSON_ERROR("You can release only revisions", 4);
		return false;
	}

	SqlQuery releaseQuery("UPDATE textrevisions SET `release` = `release` + 1 WHERE left_key <= " + left_key + " AND right_key >=" + right_key + " AND userid = " + QString::number(userid()));
	if (QMap::contains(targetRevision))
	{
		QMap::find(targetRevision)->setRelease(true);
	}

	return releaseQuery.lastError().type() == QSqlError::NoError;
}


bool TextRevisionContainer::unrelease(int targetRevision)
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	SqlQuery releaseRevInfo("SELECT left_key,right_key,`type`,`release` FROM textrevisions WHERE revisionid = " + QString::number(targetRevision));
	if (releaseRevInfo.size() != 1)
	{
		JSON_ERROR("Revision not founded", 2);
		return false;
	}
	VERIFY(releaseRevInfo.next());

	QString left_key = releaseRevInfo.value("left_key").toString();
	QString right_key = releaseRevInfo.value("right_key").toString();
	QString relType = releaseRevInfo.value("type").toString();
	int releaseVal = releaseRevInfo.value("release").toInt();

	if (releaseVal <= 0)
	{
		JSON_ERROR("Revision not founded", 3);
		return false;
	}

	if (relType != "REVISION")
	{
		JSON_ERROR("You can release only revisions", 4);
		return false;
	}

	SqlQuery unreleaseQuery("UPDATE textrevisions SET `release` = `release` - 1 WHERE left_key <= " + left_key + " AND right_key >=" + right_key + " AND userid = " + QString::number(userid()));
	if (QMap::contains(targetRevision))
	{
		QMap::find(targetRevision)->setRelease(false);
	}

	return unreleaseQuery.lastError().type() == QSqlError::NoError;
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
	SqlQuery tree = SqlQuery("SELECT * FROM textrevisions WHERE `type` != 'REVISION' ORDER BY left_key");

	QJsonArray array;
	QList<TextRevision> revisions;
	TextRevision* lastParent = nullptr;
	TextRevision* lastChild = nullptr;
	while (tree.next())
	{
		TextRevision revision;
		revision.syncRecord(tree);

		if (lastParent != nullptr && lastParent->revisionId() == revision.parent())
		{
			lastParent->appendChild(revision);
			lastChild = &lastParent->m_childs.last();
		}
		else if (lastChild != nullptr && lastChild->revisionId() == revision.parent())
		{
			lastChild->appendChild(revision);
			lastParent = lastChild;
			lastChild = &lastChild->m_childs.last();
		}
		else
		{
			revisions.append(revision);
			lastParent = &revisions.last();
		}
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
		if (selectQuery.value("type").toString() == "REVISION")
		{
			QMap::insert(revision.revisionId(), revision);
		}
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

bool TextRevisionContainer::move(int revisionId, int targetId)
{
	if (!m_user.isLogined())
	{
		JSON_ERROR("Not loginned", 1);
		return false;
	}

	SqlQuery revTest("SELECT `type`,`release`,`left_key`,`right_key` FROM textrevisions WHERE userid = " + QString::number(m_user.userid()) + " AND revisionid = " + QString::number(revisionId));
	if (revTest.size() != 1)
	{
		JSON_ERROR("Wrong size", 2);
		return false;
	}
	VERIFY(revTest.next());
	QString revType = revTest.value("type").toString();
	int revRelease = revTest.value("release").toInt();
	int revLeft = revTest.value("left_key").toInt();
	int revRight = revTest.value("right_key").toInt();

	if (revType == "REVISION")
	{
		JSON_ERROR("We cannot move revisions", 4);
		return false;
	}

	SqlQuery destTest("SELECT `type`,`release`,`left_key`,`right_key` FROM textrevisions WHERE userid = " + QString::number(m_user.userid()) + " AND revisionid = " + QString::number(targetId));
	if (destTest.size() != 1)
	{
		JSON_ERROR("Wrong size", 3);
		return false;
	}
	VERIFY(destTest.next());
	QString destType = destTest.value("type").toString();
	int destRelease = destTest.value("release").toInt();
	int destLeft = destTest.value("left_key").toInt();
	int destRight = destTest.value("right_key").toInt();

	if (destType == "REVISION" || destType == "TEXT")
	{
		JSON_ERROR("We cannot into revision or text", 5);
		return false;
	}

	SqlQuery parentTest("SELECT `" + m_parent_name + "` FROM textrevisions WHERE userid = " + QString::number(m_user.userid()) + " AND revisionid = " + QString::number(revisionId));
	VERIFY(parentTest.size() > 0);
	VERIFY(parentTest.next());
	int parent_id_now = parentTest.value(m_parent_name).toInt();
	if (parent_id_now == targetId)
	{
		JSON_ERROR("Already moved", 7);
		return false;
		// already moved
	}

	// Update realeses
	if (revRelease > 0)
	{
		SqlQuery updateRelease("UPDATE textrevisions SET `release` = `release` + " + QString::number(revRelease) + " WHERE left_key <= " + QString::number(destLeft) + " AND right_key >= " + QString::number(destRight) + " AND userid = " + QString::number(userid()));
	}

	if (destRelease > 0)
	{
		SqlQuery updateRelease("UPDATE textrevisions SET `release` = `release` - " + QString::number(revRelease) + " WHERE left_key < " + QString::number(revLeft) + " AND right_key > " + QString::number(revRight) + " AND userid = " + QString::number(userid()));
	}

	m_where_coincidence = "userid = " + QString::number(m_user.userid());
	bool status = NestedSet::move(revisionId, targetId);
	return status;
}

bool TextRevisionContainer::move(const TextRevision& revisionId, const TextRevision& targetId)
{
	return TextRevisionContainer::move(revisionId.revisionId(), targetId.revisionId());
}

}