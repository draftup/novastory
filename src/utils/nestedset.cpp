#include "nestedset.h"
#include "sql/sqlquery.h"
#include <QSqlError>
#include "utils/globals.h"

namespace novastory
{


NestedSet::NestedSet()
{

}

int NestedSet::insert(int id, const QVariant& value)
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
	insertRequest.prepare(QString("INSERT INTO `%1` SET `%2` = " + rightKey + ", `%3` = " + rightKey + " + 1, %4 = :data").arg(m_table_name).arg(m_left_name).arg(m_right_name).arg(m_data_name));
	insertRequest.bindValue(":data", value);

	status &= insertRequest.exec();

	if (status)
	{
		return insertRequest.lastInsertId().toInt();
	}

	return -1;
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

}
