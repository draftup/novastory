#include "nestedset.h"
#include "sql/sqlquery.h"
#include <QSqlError>
#include "utils/globals.h"

namespace novastory
{


NestedSet::NestedSet()
{

}

int NestedSet::insert(int id, const QHash<QString, QVariant>& values)
{
	QString rightKey;

	if (id != 0)
	{
		// Find id of right key
		SqlQuery idFind(QString("SELECT %2 FROM %1 WHERE %3 = %4").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(id));
		if (idFind.size() != 1)
		{
			return -2;
		}

		VERIFY(idFind.next());

		rightKey = idFind.value(m_right_name).toString();
	}
	else
	{
		SqlQuery maxFind(QString("SELECT MAX(%2) as maxkey FROM %1 LIMIT 1").arg(m_table_name).arg(m_right_name));
		if (maxFind.size() == 1)
		{
			VERIFY(maxFind.next());
			// New leef will be inseted out of set
			int max = maxFind.value("maxkey").toInt();
			rightKey = QString::number(max + 1);
		}
		else
		{
			rightKey = "1";
		}
	}
	VERIFY(rightKey.size() > 0);

	// First: UPDATE my_tree SET left_key = left_key + 2, right_ key = right_ key + 2 WHERE left_key > $right_ key
	// Second: UPDATE my_tree SET right_key = right_key + 2 WHERE right_key >= $right_key AND left_key < $right_key
	// Complex:
	// UPDATE my_tree SET right_key = right_key + 2, left_key = IF(left_key > $right_key, left_key + 2, left_key) WHERE right_key >= $right_key
	SqlQuery updateRequest(QString("UPDATE `%1` SET %3 = %3 + 2, %2 = IF(%2 > " + rightKey + ", %2 + 2, %2) WHERE %3 >= " + rightKey).arg(m_table_name).arg(m_left_name).arg(m_right_name));

	bool status = updateRequest.lastError().type() == QSqlError::NoError;

	SqlQuery insertRequest;
	QString insertSql = QString("INSERT INTO `%1` SET `%2` = " + rightKey + ", `%3` = " + rightKey + " + 1").arg(m_table_name).arg(m_left_name).arg(m_right_name);
	QHashIterator<QString, QVariant> it(values);
	while (it.hasNext())
	{
		it.next();
		insertSql += ", `" + it.key() + "` = :" + it.key();
	}
	if (!m_parent_name.isEmpty() && id > 0)
	{
		insertSql += ", `" + m_parent_name + "` = :" + m_parent_name;
	}
	insertRequest.prepare(insertSql);
	it.toFront();
	while (it.hasNext())
	{
		it.next();
		insertRequest.bindValue(":" + it.key(), it.value());
	}
	if (!m_parent_name.isEmpty() && id > 0)
	{
		insertRequest.bindValue(":" + m_parent_name, id);
	}

	status &= insertRequest.exec();

	if (status)
	{
		return insertRequest.lastInsertId().toInt();
	}

	return -1;
}


int NestedSet::insert(int id, const QVariant& value)
{
	QHash<QString, QVariant> values;
	values.insert(m_data_name, value);
	return insert(id, values);
}

int NestedSet::insert(const QVariant& value)
{
	return insert(0, value);
}


bool NestedSet::remove(int id)
{
	// Find id of right key
	SqlQuery idFind(QString("SELECT %2,%5 FROM %1 WHERE %3 = %4").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(id).arg(m_left_name));
	if (idFind.size() != 1)
	{
		return false;
	}

	VERIFY(idFind.next());

	QString leftKey = idFind.value(m_left_name).toString();
	QString rightKey = idFind.value(m_right_name).toString();

	// DELETE FROM my_tree WHERE left_key >= $left_key AND right_ key <= $right_key
	SqlQuery deleteQ(QString("DELETE FROM %1 WHERE %2 >= " + leftKey + " AND %3 <= " + rightKey).arg(m_table_name).arg(m_left_name).arg(m_right_name));

	if (deleteQ.lastError().type() != QSqlError::NoError)
	{
		return false;
	}

	// 1. UPDATE my_tree SET right_key = right_key – ($right_key - $left_key + 1)*** WHERE right_key > $right_key AND left_key < $left_key
	// 2. UPDATE my_tree SET left_key = left_key – ($right_key - $left_key + 1), right_key = right_key – ($right_key - $left_key + 1) WHERE left_key > $right_key
	// Complex:
	// UPDATE my_tree SET left_key = IF(left_key > $left_key, left_key – ($right_key - $left_key + 1), left_key), right_key = right_key – ($right_key - $left_key + 1) WHERE right_key > $right_key
	SqlQuery updateLeefs(QString("UPDATE %1 SET %2 = IF(%2 > " + leftKey + ", %2 - (" + rightKey + " - " + leftKey + " + 1), %2), %3 = %3 - (" + rightKey + " - " + leftKey + " + 1) WHERE %3 > " + rightKey).arg(m_table_name).arg(m_left_name).arg(m_right_name));

	return (updateLeefs.lastError().type() == QSqlError::NoError);
}


int NestedSet::leftKey(int id) const
{
	SqlQuery q(QString("SELECT * FROM %1 WHERE %2 = %3").arg(m_table_name).arg(m_id_name).arg(id));
	if (q.size() != 1)
	{
		return -1;
	}

	VERIFY(q.next());

	return q.value(m_left_name).toInt();
}

int NestedSet::rightKey(int id) const
{
	SqlQuery q(QString("SELECT * FROM %1 WHERE %2 = %3").arg(m_table_name).arg(m_id_name).arg(id));
	if (q.size() != 1)
	{
		return -1;
	}

	VERIFY(q.next());

	return q.value(m_right_name).toInt();
}

SqlQuery NestedSet::tree() const
{
	return SqlQuery(QString("SELECT * FROM %1 ORDER BY %2").arg(m_table_name).arg(m_left_name));
}

novastory::SqlQuery NestedSet::subtree(int id) const
{
	SqlQuery idFind(QString("SELECT %2,%5 FROM %1 WHERE %3 = %4").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(id).arg(m_left_name));
	if (idFind.size() != 1)
	{
		return SqlQuery();
	}

	VERIFY(idFind.next());

	QString leftKey = idFind.value(m_left_name).toString();
	QString rightKey = idFind.value(m_right_name).toString();

	return SqlQuery(QString("SELECT * FROM %1 WHERE %2 >= " + leftKey + " AND %3 <= " + rightKey + " ORDER BY %2").arg(m_table_name).arg(m_left_name).arg(m_right_name));
}

novastory::SqlQuery NestedSet::parentTree(int id) const
{
	SqlQuery idFind(QString("SELECT %2,%5 FROM %1 WHERE %3 = %4").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(id).arg(m_left_name));
	if (idFind.size() != 1)
	{
		return SqlQuery();
	}

	VERIFY(idFind.next());

	QString leftKey = idFind.value(m_left_name).toString();
	QString rightKey = idFind.value(m_right_name).toString();

	return SqlQuery(QString("SELECT * FROM %1 WHERE %2 <= " + leftKey + " AND %3 >= " + rightKey + " ORDER BY %2").arg(m_table_name).arg(m_left_name).arg(m_right_name));
}

novastory::SqlQuery NestedSet::contaisTree(int id) const
{
	SqlQuery idFind(QString("SELECT %2,%5 FROM %1 WHERE %3 = %4").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(id).arg(m_left_name));
	if (idFind.size() != 1)
	{
		return SqlQuery();
	}

	VERIFY(idFind.next());

	QString leftKey = idFind.value(m_left_name).toString();
	QString rightKey = idFind.value(m_right_name).toString();

	return SqlQuery(QString("SELECT * FROM %1 WHERE %3 > " + leftKey + " AND %2 < " + rightKey + " ORDER BY %2").arg(m_table_name).arg(m_left_name).arg(m_right_name));
}

}
