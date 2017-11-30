#ifndef DBPATCHER_H
#define DBPATCHER_H

#include <QString>
#include <QSet>
#include <QList>
#include <QHash>

class Test_DBPatcher;

namespace novastory
{

class DBPatcher
{
	friend class ::Test_DBPatcher;
public:
	struct Column
	{
		// be careful changing this struct, change dbpather same way
		QString field;
		QString type;
		bool isnull;
		QString key;
		QString default_data;
		QString extra;

		bool operator==(const Column& table) const;
		QString serialize() const;
	};

	struct Table
	{
		QString table;
		QList<Column> columns;
		QHash<QString, QList<QString>> uniq_keys;

		bool create();
		bool modify(const Table& old);

		bool operator==(const Table& table) const;
	};

	DBPatcher();
	~DBPatcher() {};

	bool patch();

	static QString cppSerialize();

	void setDatabaseStructure(const QSet<Table>& structure);
private:
	static QHash<QString, Table> columnListDB();

	QSet<Table> m_database;
};

inline uint qHash(const novastory::DBPatcher::Table& key)
{
	return qHash(key.table);
}

inline uint qHash(const novastory::DBPatcher::Column& key)
{
	return qHash(key.field);
}

}

#endif
