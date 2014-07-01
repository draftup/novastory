#ifndef DBPATCHER_H
#define DBPATCHER_H

#include <QString>
#include <QSet>

class Test_DBPatcher;

namespace novastory
{

class DBPatcher
{
	friend class ::Test_DBPatcher;
	struct Column
	{
		QString field;
		QString type;
		bool isnull;
		QString key;
		QString default;
		QString extra;
	};
public:
	struct Table
	{
		QString table;
		QList<Column> columns;

		bool create();

		bool operator==(const Table& table) const;
	};

	DBPatcher();
	~DBPatcher() {};

	bool patch();
private:

	QSet<Table> m_database;
};

}

inline uint qHash(const novastory::DBPatcher::Table& key)
{
	return qHash(key.table);
}

#endif
