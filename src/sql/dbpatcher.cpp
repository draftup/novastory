#include "dbpatcher.h"
#include "sqldatabase.h"
#include "sqlquery.h"
#include "config.h"
#include <QSet>

namespace novastory
{
DBPatcher::DBPatcher()
{

}

bool DBPatcher::patch()
{
	bool status = true;

	SqlQuery tables("select TABLE_NAME from  information_schema.tables where table_schema = '" MYSQL_DATABASE "'");
	QSet<Table> tablesList;
	while (tables.next())
	{
		Table table;
		table.table = tables.value("TABLE_NAME").toString();
		tablesList.insert(table);
	}

	QSet<Table> newTables = m_database - tablesList;
	for (Table newTable : newTables)
	{
		status &= newTable.create();
	}

	SqlQuery fields("select * from  information_schema.columns where table_schema = '" MYSQL_DATABASE "'");

	SqlDatabase::close();

	return status;
}


bool DBPatcher::Table::create()
{
	QString sql = "CREATE TABLE " + table + "(";

	QListIterator<Column> it(columns);
	while (it.hasNext())
	{
		Column column = it.next();
		sql += QString("\n`%1` %2").arg(column.field).arg(column.type);
		if (!column.isnull)
		{
			sql += " NOT NULL";
		}
		if (column.default != "NULL" && !column.default.isEmpty())
		{
			sql += " DEFAULT " + column.default;
		}
		if (!column.extra.isEmpty())
		{
			sql += " " + column.extra;
		}

		if (column.key == "PRI")
		{
			sql += QString(",\nPRIMARY KEY(`%1`)").arg(column.field);
		}

		if (it.hasNext())
		{
			sql += ",";
		}
	}

	sql += "\n)";

	SqlQuery query;
	bool status = query.exec(sql);
	if (status)
	{
		qDebug() << "New table '" + table + "' created";
	}
	else
	{
		qCritical() << "Failed to create '" + table + "'";
	}
	return status;
}

bool DBPatcher::Table::operator==(const Table& t) const
{
	return table == t.table;
}

}


