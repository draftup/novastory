#ifndef NESTEDSET_H
#define NESTEDSET_H

#include <QString>
#include <QVariant>

class Test_NestedSet;

namespace novastory
{

class NestedSet
{
	friend class ::Test_NestedSet;
public:
	NestedSet();

	int insert(int id, const QVariant& value);
	bool remove(int id);
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
