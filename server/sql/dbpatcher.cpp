#include "dbpatcher.h"
#include "sqldatabase.h"
#include "sqlquery.h"
#include "config.h"
#include <QSet>
#include <QMutableSetIterator>

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


	// we don't need to delete extension tables
	QHash<QString, Table> columnList = columnListDB();

	// Modify all tables
	for (Table table : m_database - newTables)
	{
		if (!columnList.contains(table.table))
		{
			qCritical() << "No table after first part? Lol! Something wrong.";
			continue;
		}

		status &= table.modify(columnList[table.table]);
	}

	SqlDatabase::close();
	return status;
}

QString DBPatcher::cppSerialize()
{
	QString cpp = "#ifndef NOVASTORY_DB_H\n#define NOVASTORY_DB_H\n\n"
				  "#include \"sql/dbpatcher.h\"\n\n"
				  "namespace novastory {\n\n"
				  "QSet<DBPatcher::Table> DB_TABLE_STRUCT()\n{\n QSet<DBPatcher::Table> TABLE_STRUCT;\n"
				  ;

	QHash<QString, Table> columnList = columnListDB();

	for (const Table& table : columnList)
	{
		cpp += " TABLE_STRUCT << DBPatcher::Table{\n";
		cpp += QString("  \"%1\",\n QList<DBPatcher::Column>({\n").arg(table.table);
		for (const Column& column : table.columns)
		{
			cpp += "   DBPatcher::Column{\n";

			cpp += QString("    \"%1\",\n").arg(column.field);
			cpp += QString("    \"%1\",\n").arg(column.type);
			cpp += column.isnull ? "    true,\n" : "    false,\n";
			cpp += QString("    \"%1\",\n").arg(column.key);
			cpp += QString("    \"%1\",\n").arg(column.default_data);
			cpp += QString("    \"%1\"\n").arg(column.extra);

			cpp += "   },\n";
		}
		cpp += "  }),\n";

		cpp += "QHash<QString, QList<QString>>{";
		QHashIterator<QString, QList<QString>> it(table.uniq_keys);
		while (it.hasNext())
		{
			it.next();
			cpp += QString("{\"%1\", QList<QString>{").arg(it.key());
			for (const QString& k : it.value())
			{
				cpp += "\"" + k + "\", ";
			}
			cpp += "}}, ";
		}
		cpp += "}\n";

		cpp += "};\n";
	}

	cpp += " return TABLE_STRUCT;\n}\n\n}\n\n#endif // NOVASTORY_DB_H"; // close namespace

	return cpp;
}

QHash<QString, DBPatcher::Table> DBPatcher::columnListDB()
{
	SqlQuery fields("select * from information_schema.columns where table_schema = '" MYSQL_DATABASE "'");
	QHash<QString, Table> tableList;
	while (fields.next())
	{
		QString name = fields.value("TABLE_NAME").toString();
		if (!tableList.contains(name))
		{
			Table tb;
			tb.table = name;
			tableList[name] = tb;
		}

		Table& table = tableList[name];

		Column newColumn;
		newColumn.field = fields.value("COLUMN_NAME").toString();
		newColumn.type = fields.value("COLUMN_TYPE").toString();
		newColumn.isnull = fields.value("IS_NULLABLE").toString() == "YES";
		newColumn.default_data = fields.value("COLUMN_DEFAULT").toString();
		newColumn.key = fields.value("COLUMN_KEY").toString();
		newColumn.extra = fields.value("EXTRA").toString();

		table.columns.append(newColumn);
	}

	SqlQuery uniq_keys("select * from information_schema.key_column_usage where table_schema = '" MYSQL_DATABASE "' and CONSTRAINT_NAME != 'PRIMARY'");
	while (uniq_keys.next())
	{
		QString table = uniq_keys.value("TABLE_NAME").toString();
		QString uniq_name = uniq_keys.value("CONSTRAINT_NAME").toString();
		QString uniq_cont = uniq_keys.value("COLUMN_NAME").toString();

		Table& tab = tableList[table];
		tab.uniq_keys[uniq_name].append(uniq_cont);
	}

	return tableList;
}

void DBPatcher::setDatabaseStructure(const QSet<Table>& structure)
{
	m_database = structure;
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
		else if (column.key == "MUL")
		{
			sql += QString(",\nKEY(`%1`)").arg(column.field);
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

	QList<QString> uniColums;
	QList<QString> uniColumsOld;
	for (const QList<QString>& keys : old.uniq_keys.values())
	{
		uniColumsOld += keys;
	}
	for (const QList<QString>& keys : uniq_keys.values())
	{
		uniColums += keys;
	}

	QSet<Column> columnsSet = columns.toSet();
	QSet<Column> oldColumnsSet = old.columns.toSet();

	QSet<Column> newColumns = columnsSet - oldColumnsSet;
	for (const Column& column : newColumns)
	{
		SqlQuery query;
		status &= query.exec(QString("ALTER TABLE `%1` ADD %2").arg(this->table).arg(column.serialize()));
		if (status)
		{
			qDebug() << "Add new column '" << column.field << "'";
		}
		else
		{
			qCritical() << "Failed add new column '" << column.field << "'";
		}
	}

	QSet<Column> removeColumns = oldColumnsSet - columnsSet;
	for (const Column& column : removeColumns)
	{
		SqlQuery query;
		status &= query.exec(QString("ALTER TABLE `%1` DROP `%2`").arg(this->table).arg(column.field));
		if (status)
		{
			qDebug() << "Remove column '" << column.field << "'";
		}
		else
		{
			qCritical() << "Failed remove column '" << column.field << "'";
		}
	}

	// QSet intersection can reverse target and base qset. To overcome this we will do set intersection manually
	QMutableSetIterator<Column> it(columnsSet);
	while (it.hasNext())
	{
		const Column& cl = it.next();
		if (!oldColumnsSet.contains(cl))
		{
			it.remove();
		}
	}

	for (const Column& column : columnsSet)
	{
		Column findC;
		findC.field = column.field;
		Column oldColumn = *oldColumnsSet.find(findC);

		if (
			column.type != oldColumn.type ||
			column.isnull != oldColumn.isnull ||
			column.default_data != oldColumn.default_data ||
			column.extra != oldColumn.extra)
		{
			SqlQuery query;
			status &= query.exec(QString("ALTER TABLE `%1` MODIFY %2").arg(this->table).arg(column.serialize()));
			if (status)
			{
				qDebug() << "Modify column type '" << column.field << "' from " << oldColumn.type << " to " << column.type;
			}
			else
			{
				qCritical() << "Failed modify column type '" << column.field << "' from " << oldColumn.type << " to " << column.type;
			}
		}

		if (column.key != oldColumn.key)
		{
			if (!oldColumn.key.isEmpty())
			{
				SqlQuery query;
				if (oldColumn.key == "PRI")
				{
					status &= query.exec(QString("ALTER TABLE `%1` DROP PRIMARY KEY").arg(this->table));
					if (status)
					{
						qDebug() << "Drop primary key '" << column.field;
					}
					else
					{
						qCritical() << "Failed drop primary key '" << column.field;;
					}
				}
				else if (!uniColumsOld.contains(column.field))
				{
					status &= query.exec(QString("ALTER TABLE `%1` DROP KEY `%2`").arg(this->table).arg(column.field));
					if (status)
					{
						qDebug() << "Drop key '" << column.field;
					}
					else
					{
						qCritical() << "Failed drop key '" << column.field;;
					}
				}
			}

			if (!column.key.isEmpty())
			{
				SqlQuery query;
				if (column.key == "PRI")
				{
					status &= query.exec(QString("ALTER TABLE `%1` ADD PRIMARY KEY(`%2`)").arg(this->table).arg(column.field));
					if (status)
					{
						qDebug() << "Created primary key '" << column.field;
					}
					else
					{
						qCritical() << "Failed to create primary key '" << column.field;
					}
				}
				else if (column.key == "MUL" && !uniColums.contains(column.field))
				{
					status &= query.exec(QString("ALTER TABLE `%1` ADD KEY(`%2`)").arg(this->table).arg(column.field));
					if (status)
					{
						qDebug() << "Created key '" << column.field;
					}
					else
					{
						qCritical() << "Failed to create key '" << column.field;
					}
				}
				else if (!uniColums.contains(column.field))
				{
					qWarning() << "Unsupported key type " << column.key << "for patching";
					continue;
				}
			}
		}
	}

	// Create/delete unique fields
	if (uniq_keys != old.uniq_keys)
	{
		QSet<QString> new_ukeys = uniq_keys.keys().toSet() - old.uniq_keys.keys().toSet();
		for (const QString& key : new_ukeys)
		{
			QString sql = QString("ALTER TABLE `%1` ADD UNIQUE KEY `%2`(").arg(this->table).arg(key);
			QListIterator<QString> it(uniq_keys[key]);
			while (it.hasNext())
			{
				sql += "`" + it.next() + "`";
				if (it.hasNext())
				{
					sql += ",";
				}
			}
			sql += ")";
			status &= SqlQuery().exec(sql);
			if (status)
			{
				qDebug() << "Unique key" << key << "added";
			}
			else
			{
				qCritical() << "Failed on key add";
			}
		}

		QSet<QString> old_ukeys = old.uniq_keys.keys().toSet() - uniq_keys.keys().toSet();
		for (const QString& key : old_ukeys)
		{
			status &= SqlQuery().exec(QString("ALTER TABLE `%1` DROP KEY `%2`").arg(this->table).arg(key));
			if (status)
			{
				qDebug() << "Unique key" << key << "removed";
			}
			else
			{
				qCritical() << "Failed on key remove";
			}
		}
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
	if (default_data != "NULL" && !default_data.isEmpty())
	{
		if (default_data == "CURRENT_TIMESTAMP")
		{
			sql += " DEFAULT " + default_data;
		}
		else
		{
			sql += " DEFAULT '" + default_data + "'";
		}
	}
	if (!extra.isEmpty())
	{
		sql += " " + extra;
	}

	return sql;
}

}


