#ifndef NESTEDSET_H
#define NESTEDSET_H

#include <QString>
#include <QVariant>
#include "sql/sqlquery.h"

class Test_NestedSet;

namespace novastory
{

class NestedSet
{
	friend class ::Test_NestedSet;
public:
	NestedSet();

	int insert(int id, const QVariant& value);
	int insert(const QVariant& value);
	bool remove(int id);

	SqlQuery tree() const;
	SqlQuery subtree(int id) const;
	SqlQuery parentTree(int id) const;
	SqlQuery contaisTree(int id) const;
private:
	int leftKey(int id) const;
	int rightKey(int id) const;
protected:
	QString m_left_name;
	QString m_right_name;
	QString m_table_name;
	QString m_data_name;
	QString m_id_name;
};

}

#endif
