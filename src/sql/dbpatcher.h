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
public:
	struct Column
	{
		QString field;
		QString type;
		bool isnull;
		QString key;
		QString default;
		QString extra;

		bool operator==(const Column& table) const;
		QString serialize() const;
	};

	struct Table
	{
		QString table;
		QList<Column> columns;

		bool create();
		bool modify(const Table& old);

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

inline uint qHash(const novastory::DBPatcher::Column& key)
{
	return qHash(key.field);
}

#endif
