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
		QString coincidenceString;
		if (!m_where_coincidence.isEmpty())
		{
			coincidenceString = " AND " + m_where_coincidence;
		}
		SqlQuery idFind(QString("SELECT %2 FROM %1 WHERE %3 = %4%5").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(id).arg(coincidenceString));
		if (idFind.size() != 1)
		{
			return -2;
		}

		VERIFY(idFind.next());

		rightKey = idFind.value(m_right_name).toString();
	}
	else
	{
		QString coincidenceString;
		if (!m_where_coincidence.isEmpty())
		{
			coincidenceString = " WHERE " + m_where_coincidence;
		}
		SqlQuery maxFind(QString("SELECT MAX(%2) as maxkey FROM %1%3 LIMIT 1").arg(m_table_name).arg(m_right_name).arg(coincidenceString));
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

	QString coincidenceString;
	if (!m_where_coincidence.isEmpty())
	{
		coincidenceString = " AND " + m_where_coincidence;
	}

	// First: UPDATE my_tree SET left_key = left_key + 2, right_ key = right_ key + 2 WHERE left_key > $right_ key
	// Second: UPDATE my_tree SET right_key = right_key + 2 WHERE right_key >= $right_key AND left_key < $right_key
	// Complex:
	// UPDATE my_tree SET right_key = right_key + 2, left_key = IF(left_key > $right_key, left_key + 2, left_key) WHERE right_key >= $right_key
	SqlQuery updateRequest(QString("UPDATE `%1` SET %3 = %3 + 2, %2 = IF(%2 > " + rightKey + ", %2 + 2, %2) WHERE %3 >= " + rightKey + "%4").arg(m_table_name).arg(m_left_name).arg(m_right_name).arg(coincidenceString));

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
	QString coincidenceString;
	if (!m_where_coincidence.isEmpty())
	{
		coincidenceString = " AND " + m_where_coincidence;
	}

	// Find id of right key
	SqlQuery idFind(QString("SELECT %2,%5 FROM %1 WHERE %3 = %4%6").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(id).arg(m_left_name).arg(coincidenceString));
	if (idFind.size() != 1)
	{
		return false;
	}

	VERIFY(idFind.next());

	QString leftKey = idFind.value(m_left_name).toString();
	QString rightKey = idFind.value(m_right_name).toString();

	// DELETE FROM my_tree WHERE left_key >= $left_key AND right_ key <= $right_key
	SqlQuery deleteQ(QString("DELETE FROM %1 WHERE %2 >= " + leftKey + " AND %3 <= " + rightKey + "%4").arg(m_table_name).arg(m_left_name).arg(m_right_name).arg(coincidenceString));

	if (deleteQ.lastError().type() != QSqlError::NoError)
	{
		return false;
	}

	// 1. UPDATE my_tree SET right_key = right_key – ($right_key - $left_key + 1)*** WHERE right_key > $right_key AND left_key < $left_key
	// 2. UPDATE my_tree SET left_key = left_key – ($right_key - $left_key + 1), right_key = right_key – ($right_key - $left_key + 1) WHERE left_key > $right_key
	// Complex:
	// UPDATE my_tree SET left_key = IF(left_key > $left_key, left_key – ($right_key - $left_key + 1), left_key), right_key = right_key – ($right_key - $left_key + 1) WHERE right_key > $right_key
	SqlQuery updateLeefs(QString("UPDATE %1 SET %2 = IF(%2 > " + leftKey + ", %2 - (" + rightKey + " - " + leftKey + " + 1), %2), %3 = %3 - (" + rightKey + " - " + leftKey + " + 1) WHERE %3 > " + rightKey + "%4").arg(m_table_name).arg(m_left_name).arg(m_right_name).arg(coincidenceString));

	return (updateLeefs.lastError().type() == QSqlError::NoError);
}


int NestedSet::leftKey(int id) const
{
	QString coincidenceString;
	if (!m_where_coincidence.isEmpty())
	{
		coincidenceString = " AND " + m_where_coincidence;
	}
	SqlQuery q(QString("SELECT * FROM %1 WHERE %2 = %3%4").arg(m_table_name).arg(m_id_name).arg(id).arg(coincidenceString));
	if (q.size() != 1)
	{
		return -1;
	}

	VERIFY(q.next());

	return q.value(m_left_name).toInt();
}

int NestedSet::rightKey(int id) const
{
	QString coincidenceString;
	if (!m_where_coincidence.isEmpty())
	{
		coincidenceString = " AND " + m_where_coincidence;
	}
	SqlQuery q(QString("SELECT * FROM %1 WHERE %2 = %3%4").arg(m_table_name).arg(m_id_name).arg(id).arg(coincidenceString));
	if (q.size() != 1)
	{
		return -1;
	}

	VERIFY(q.next());

	return q.value(m_right_name).toInt();
}

SqlQuery NestedSet::tree() const
{
	QString coincidenceString;
	if (!m_where_coincidence.isEmpty())
	{
		coincidenceString = " WHERE " + m_where_coincidence;
	}
	return SqlQuery(QString("SELECT * FROM %1%3 ORDER BY %2").arg(m_table_name).arg(m_left_name).arg(coincidenceString));
}

novastory::SqlQuery NestedSet::subtree(int id) const
{
	QString coincidenceString;
	if (!m_where_coincidence.isEmpty())
	{
		coincidenceString = " AND " + m_where_coincidence;
	}
	SqlQuery idFind(QString("SELECT %2,%5 FROM %1 WHERE %3 = %4%6").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(id).arg(m_left_name).arg(coincidenceString));
	if (idFind.size() != 1)
	{
		return SqlQuery();
	}

	VERIFY(idFind.next());

	QString leftKey = idFind.value(m_left_name).toString();
	QString rightKey = idFind.value(m_right_name).toString();

	return SqlQuery(QString("SELECT * FROM %1 WHERE %2 >= " + leftKey + " AND %3 <= " + rightKey + "%4 ORDER BY %2").arg(m_table_name).arg(m_left_name).arg(m_right_name).arg(coincidenceString));
}

novastory::SqlQuery NestedSet::parentTree(int id) const
{
	QString coincidenceString;
	if (!m_where_coincidence.isEmpty())
	{
		coincidenceString = " AND " + m_where_coincidence;
	}
	SqlQuery idFind(QString("SELECT %2,%5 FROM %1 WHERE %3 = %4%6").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(id).arg(m_left_name).arg(coincidenceString));
	if (idFind.size() != 1)
	{
		return SqlQuery();
	}

	VERIFY(idFind.next());

	QString leftKey = idFind.value(m_left_name).toString();
	QString rightKey = idFind.value(m_right_name).toString();

	return SqlQuery(QString("SELECT * FROM %1 WHERE %2 <= " + leftKey + " AND %3 >= " + rightKey + "%4 ORDER BY %2").arg(m_table_name).arg(m_left_name).arg(m_right_name).arg(coincidenceString));
}

novastory::SqlQuery NestedSet::contaisTree(int id) const
{
	QString coincidenceString;
	if (!m_where_coincidence.isEmpty())
	{
		coincidenceString = " AND " + m_where_coincidence;
	}
	SqlQuery idFind(QString("SELECT %2,%5 FROM %1 WHERE %3 = %4%6").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(id).arg(m_left_name).arg(coincidenceString));
	if (idFind.size() != 1)
	{
		return SqlQuery();
	}

	VERIFY(idFind.next());

	QString leftKey = idFind.value(m_left_name).toString();
	QString rightKey = idFind.value(m_right_name).toString();

	return SqlQuery(QString("SELECT * FROM %1 WHERE %3 > " + leftKey + " AND %2 < " + rightKey + "%4 ORDER BY %2").arg(m_table_name).arg(m_left_name).arg(m_right_name).arg(coincidenceString));
}

novastory::SqlQuery NestedSet::leefs() const
{
	QString coincidenceString;
	if (!m_where_coincidence.isEmpty())
	{
		coincidenceString = " AND " + m_where_coincidence;
	}

	return SqlQuery(QString("SELECT * FROM %1 WHERE %3 - %2 = 1%4 ORDER BY %2").arg(m_table_name).arg(m_left_name).arg(m_right_name).arg(coincidenceString));

}

novastory::SqlQuery NestedSet::notLeefs() const
{
	QString coincidenceString;
	if (!m_where_coincidence.isEmpty())
	{
		coincidenceString = " AND " + m_where_coincidence;
	}

	return SqlQuery(QString("SELECT * FROM %1 WHERE %3 - %2 > 1%4 ORDER BY %2").arg(m_table_name).arg(m_left_name).arg(m_right_name).arg(coincidenceString));
}

bool NestedSet::move(int id, int parent_id)
{
	QString coincidenceString;
	if (!m_where_coincidence.isEmpty())
	{
		coincidenceString = " AND " + m_where_coincidence;
	}
	SqlQuery idFind(QString("SELECT %2,%5 FROM %1 WHERE %3 = %4%6").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(id).arg(m_left_name).arg(coincidenceString));
	if (idFind.size() != 1)
	{
		return false;
	}
	VERIFY(idFind.next());

	int moving_left = idFind.value(m_left_name).toInt();
	int moving_right = idFind.value(m_right_name).toInt();
	int moving_size = moving_right - moving_left + 1;

	SqlQuery parentFind(QString("SELECT %2,%3 FROM %1 WHERE %3 = %4%6").arg(m_table_name).arg(m_right_name).arg(m_id_name).arg(parent_id).arg(coincidenceString));
	if (parentFind.size() != 1)
	{
		return false;
	}
	VERIFY(parentFind.next());

	int parent_id_ = parentFind.value(m_id_name).toInt();
	int parent_right = parentFind.value(m_right_name).toInt();

	// update all moving tree to negate
	/*
	UPDATE `list_items`
	SET `pos_left` = 0-(`pos_left`), `pos_right` = 0-(`pos_right`)
	WHERE `pos_left` >= @node_pos_left AND `pos_right` <= @node_pos_right;
	*/
	SqlQuery disableTree(
		QString("UPDATE %1 "
				"SET `%2` = 0 - (`%2`), `%3` = 0 - (`%3`) "
				"WHERE `%2` >= " + QString::number(moving_left) + " AND `%3` <= " + QString::number(moving_right) + "%4")
				.arg(m_table_name).arg(m_left_name).arg(m_right_name).arg(coincidenceString)
	);

	if (disableTree.lastError().type() != QSqlError::NoError)
	{
		return false;
	}

	/*
	UPDATE `list_items`
	SET `pos_left` = `pos_left` - @node_size
	WHERE `pos_left` > @node_pos_right;
	UPDATE `list_items`
	SET `pos_right` = `pos_right` - @node_size
	WHERE `pos_right` > @node_pos_right;
	*/
	SqlQuery restrictFullTree1(
		QString(QString("UPDATE %1")
				+ " SET `%2` = `%2` - " + QString::number(moving_size)
				+ " WHERE `%2` > " + QString::number(moving_right)
				+ "%3"
				).arg(m_table_name).arg(m_left_name).arg(coincidenceString)
	);
	if (restrictFullTree1.lastError().type() != QSqlError::NoError)
	{
		return false;
	}
	SqlQuery restrictFullTree2(
		QString(QString("UPDATE %1")
				+ " SET `%2` = `%2` - " + QString::number(moving_size)
				+ " WHERE `%2` > " + QString::number(moving_right)
				+ "%3"
				).arg(m_table_name).arg(m_right_name).arg(coincidenceString)
	);
	if (restrictFullTree2.lastError().type() != QSqlError::NoError)
	{
		return false;
	}

	/*
	UPDATE `list_items`
	SET `pos_left` = `pos_left` + @node_size
	WHERE `pos_left` >= IF(@parent_pos_right > @node_pos_right, @parent_pos_right - @node_size, @parent_pos_right);
	UPDATE `list_items`
	SET `pos_right` = `pos_right` + @node_size
	WHERE `pos_right` >= IF(@parent_pos_right > @node_pos_right, @parent_pos_right - @node_size, @parent_pos_right);
	*/
	SqlQuery addFullTree1(
		QString(
			"UPDATE %1 "
			"SET %2 = %2 + " + QString::number(moving_size) + " "
			"WHERE %2 >= IF(" + QString::number(parent_right) + " > " + QString::number(moving_right) + ", " + QString::number(parent_right) + " - " + QString::number(moving_size) + ", " + QString::number(parent_right) + ") "
			"%3"
			).arg(m_table_name).arg(m_left_name).arg(coincidenceString)
	);
	if (addFullTree1.lastError().type() != QSqlError::NoError)
	{
		return false;
	}
	SqlQuery addFullTree2(
		QString(
			"UPDATE %1 "
			"SET %2 = %2 + " + QString::number(moving_size) + " "
			"WHERE %2 >= IF(" + QString::number(parent_right) + " > " + QString::number(moving_right) + ", " + QString::number(parent_right) + " - " + QString::number(moving_size) + ", " + QString::number(parent_right) + ") "
			"%3"
			).arg(m_table_name).arg(m_right_name).arg(coincidenceString)
	);
	if (addFullTree2.lastError().type() != QSqlError::NoError)
	{
		return false;
	}

	/*
	UPDATE `list_items`
	SET
	`pos_left` = 0-(`pos_left`)+IF(@parent_pos_right > @node_pos_right, @parent_pos_right - @node_pos_right - 1, @parent_pos_right - @node_pos_right - 1 + @node_size),
	`pos_right` = 0-(`pos_right`)+IF(@parent_pos_right > @node_pos_right, @parent_pos_right - @node_pos_right - 1, @parent_pos_right - @node_pos_right - 1 + @node_size)
	WHERE `pos_left` <= 0-@node_pos_left AND `pos_right` >= 0-@node_pos_right;
	UPDATE `list_items`
	SET `parent_item_id` = @parent_id
	WHERE `item_id` = @node_id;
	*/
	SqlQuery backNode(
		QString(
			"UPDATE %1"
			" SET"
			" `%2` = 0 - (`%2`) + IF(" + QString::number(parent_right) + " > " + QString::number(moving_right) + ", " + QString::number(parent_right) + " - " + QString::number(moving_right) + " - 1, " + QString::number(parent_right) + " - " + QString::number(moving_right) + " - 1 + " + QString::number(moving_size) + "),"
			" `%3` = 0 - (`%3`) + IF(" + QString::number(parent_right) + " > " + QString::number(moving_right) + ", " + QString::number(parent_right) + " - " + QString::number(moving_right) + " - 1, " + QString::number(parent_right) + " - " + QString::number(moving_right) + " - 1 + " + QString::number(moving_size) + ")"
			" WHERE `%2` <= 0 - " + QString::number(moving_left) + " AND `%3` >= 0 - " + QString::number(moving_right)
			+ "%4"
			).arg(m_table_name).arg(m_left_name).arg(m_right_name).arg(coincidenceString)
	);

	if (backNode.lastError().type() != QSqlError::NoError)
	{
		return false;
	}

	SqlQuery updateID(
		QString(
			QString("UPDATE `%1`")
			+ " SET `" + m_parent_name + "` = " + QString::number(parent_id)
			+ " WHERE `" + m_id_name + "` = " + QString::number(id)
			+ "%2"
			).arg(m_table_name).arg(coincidenceString)
	);

	if (updateID.lastError().type() != QSqlError::NoError)
	{
		return false;
	}

	return true;
}

}
