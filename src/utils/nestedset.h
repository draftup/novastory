#ifndef NESTEDSET_H
#define NESTEDSET_H

#include <QString>
#include <QVariant>

namespace novastory
{

class NestedSet
{
public:
	NestedSet();

	bool insert(int parent_right_key, const QVariant& value);
private:
	QString m_left_name;
	QString m_right_name;
	QString m_table_name;
};

}

#endif
