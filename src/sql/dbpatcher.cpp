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
	/*
	QSet<Column> columnList;
	while (tables.next())
	{
		Table table;
		table.table = tables.value("TABLE_NAME").toString();
		tablesList.insert(table);
	}
	*/

	SqlDatabase::close();
	return status;
}


bool DBPatcher::Table::create()
{
	QString sql = "CREATE TABLE `" + table + "`(";

	QListIterator<Column> it(columns);
	while (it.hasNext())
	{
		Column column = it.next();
		sql += "\n" + column.serialize();

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

bool DBPatcher::Table::modify(const Table& old)
{
	bool status = true;
	QSet<Column> newColumns = columns.toSet() - old.columns.toSet();
	for (const Column& column : newColumns)
	{
		SqlQuery query;
		status &= query.exec(QString("ALTER TABLE ADD `%1`").arg(column.serialize()));
		if (status)
			qDebug() << "Add new column '" << column.field << "'";
		else
			qCritical() << "Failed add new column '" << column.field << "'";
	}

	QSet<Column> removeColumns = old.columns.toSet() - columns.toSet();
	for (const Column& column : removeColumns)
	{
		SqlQuery query;
		status &= query.exec(QString("ALTER TABLE DROP `%1`").arg(column.field));
		if (status)
			qDebug() << "Add new column '" << column.field << "'";
		else
			qCritical() << "Failed add new column '" << column.field << "'";
	}

	return status;
}


bool DBPatcher::Column::operator==(const Column& table) const
{
	return table.field == field;
}

QString DBPatcher::Column::serialize() const
{
	QString sql = QString("`%1` %2").arg(field).arg(type);
	if (!isnull)
	{
		sql += " NOT NULL";
	}
	if (default != "NULL" && !default.isEmpty())
	{
		sql += " DEFAULT " + default;
	}
	if (!extra.isEmpty())
	{
		sql += " " + extra;
	}

	return sql;
}

}


