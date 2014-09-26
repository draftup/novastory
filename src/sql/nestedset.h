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
	int insert(int id, const QHash<QString, QVariant>& values);
	int insert(const QVariant& value);
	bool remove(int id);
	bool move(int id, int parent_id);

	SqlQuery tree() const;
	SqlQuery subtree(int id) const;
	SqlQuery parentTree(int id) const;
	SqlQuery contaisTree(int id) const;
	SqlQuery leefs() const;
	SqlQuery notLeefs() const;
private:
	int leftKey(int id) const;
	int rightKey(int id) const;
protected:
	QString m_left_name;
	QString m_right_name;
	QString m_table_name;
	QString m_data_name;
	QString m_id_name;
	QString m_where_coincidence;
	QString m_parent_name;
};

}

#endif
