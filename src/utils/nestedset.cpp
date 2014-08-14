#include "nestedset.h"
#include "sql/sqlquery.h"
#include <QSqlError>

namespace novastory
{


NestedSet::NestedSet()
{

}

bool NestedSet::insert(int parent_right_key, const QVariant& value)
{
	QString rightKey = QString::number(parent_right_key);

	// First: UPDATE my_tree SET left_key = left_key + 2, right_ key = right_ key + 2 WHERE left_key > $right_ key
	// Second: UPDATE my_tree SET right_key = right_key + 2 WHERE right_key >= $right_key AND left_key < $right_key
	// Complex:
	// UPDATE my_tree SET right_key = right_key + 2, left_key = IF(left_key > $right_key, left_key + 2, left_key) WHERE right_key >= $right_key
	SqlQuery updateRequest(QString("UPDATE `%1` SET %3 = %3 + 2, left_key = IF(%2 > " + rightKey + ", %2 + 2, %2) WHERE %3 >= " + rightKey).arg(m_table_name).arg(m_left_name).arg(m_right_name));

	bool status = updateRequest.lastError().type() == QSqlError::NoError;

	SqlQuery insertRequest;
	insertRequest.prepare(QString("INSERT INTO `%1` SET `%2` = " + rightKey + ", `%3` = " + rightKey + " + 1").arg(m_table_name).arg(m_left_name).arg(m_right_name));

	status &= insertRequest.exec();

	return status;
}

}
