#ifndef NOVASTORY_DB_H
#define NOVASTORY_DB_H

#include "sql/dbpatcher.h"

namespace novastory
{

QSet<DBPatcher::Table> DB_TABLE_STRUCT()
{
	QSet<DBPatcher::Table> TABLE_STRUCT;
	TABLE_STRUCT << DBPatcher::Table
	{
		"avatars",
		QList<DBPatcher::Column>({
			DBPatcher::Column{
				"userid",
				"int(10) unsigned",
				false,
				"PRI",
				"",
				""
			},
			DBPatcher::Column{
				"data",
				"mediumblob",
				true,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"contenttype",
				"varchar(64)",
				true,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"contentsize",
				"int(10) unsigned",
				true,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"modifydate",
				"timestamp",
				false,
				"",
				"CURRENT_TIMESTAMP",
				"on update CURRENT_TIMESTAMP"
			},
		})
	};
	TABLE_STRUCT << DBPatcher::Table
	{
		"texteditor",
		QList<DBPatcher::Column>({
			DBPatcher::Column{
				"userid",
				"int(10) unsigned",
				false,
				"PRI",
				"",
				""
			},
			DBPatcher::Column{
				"text",
				"mediumtext",
				true,
				"",
				"",
				""
			},
		})
	};
	TABLE_STRUCT << DBPatcher::Table
	{
		"textrevisions",
		QList<DBPatcher::Column>({
			DBPatcher::Column{
				"revisionid",
				"int(10) unsigned",
				false,
				"PRI",
				"",
				"auto_increment"
			},
			DBPatcher::Column{
				"userid",
				"int(10) unsigned",
				false
			},
			DBPatcher::Column{
				"release",
				"tinyint(1)",
				false,
				"",
				"0"
			},
			DBPatcher::Column{
				"text",
				"mediumtext",
				true
			},
		})
	};
	TABLE_STRUCT << DBPatcher::Table
	{
		"userspassforgot",
		QList<DBPatcher::Column>({
			DBPatcher::Column{
				"token",
				"char(32)",
				false,
				"PRI",
				"",
				""
			},
			DBPatcher::Column{
				"userid",
				"int(10) unsigned",
				false,
				"MUL",
				"",
				""
			},
			DBPatcher::Column{
				"newpassword",
				"char(32)",
				false,
				"",
				"",
				""
			},
		})
	};
	TABLE_STRUCT << DBPatcher::Table
	{
		"users",
		QList<DBPatcher::Column>({
			DBPatcher::Column{
				"userid",
				"int(10) unsigned",
				false,
				"PRI",
				"",
				"auto_increment"
			},
			DBPatcher::Column{
				"password",
				"char(32)",
				false,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"salt",
				"char(32)",
				false,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"email",
				"char(100)",
				false,
				"MUL",
				"",
				""
			},
			DBPatcher::Column{
				"joindate",
				"timestamp",
				false,
				"",
				"CURRENT_TIMESTAMP",
				"on update CURRENT_TIMESTAMP"
			},
			DBPatcher::Column{
				"firstname",
				"varchar(64)",
				true,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"lastname",
				"varchar(64)",
				true,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"nickname",
				"varchar(64)",
				true,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"profileid",
				"varchar(64)",
				true,
				"MUL",
				"",
				""
			},
		})
	};
	TABLE_STRUCT << DBPatcher::Table
	{
		"subscriptions",
		QList<DBPatcher::Column>({
			DBPatcher::Column{
				"userid",
				"int(10) unsigned",
				true,
				"MUL",
				"",
				""
			},
			DBPatcher::Column{
				"targetid",
				"int(10) unsigned",
				true,
				"MUL",
				"",
				""
			},
			DBPatcher::Column{
				"options",
				"int(10) unsigned",
				false,
				"",
				"0",
				""
			},
		})
	};
	TABLE_STRUCT << DBPatcher::Table
	{
		"userspics",
		QList<DBPatcher::Column>({
			DBPatcher::Column{
				"userid",
				"int(10) unsigned",
				false,
				"PRI",
				"",
				""
			},
			DBPatcher::Column{
				"data",
				"mediumblob",
				true,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"contenttype",
				"varchar(64)",
				true,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"contentsize",
				"int(10) unsigned",
				true,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"modifydate",
				"timestamp",
				false,
				"",
				"CURRENT_TIMESTAMP",
				"on update CURRENT_TIMESTAMP"
			},
		})
	};
	TABLE_STRUCT << DBPatcher::Table
	{
		"usersverify",
		QList<DBPatcher::Column>({
			DBPatcher::Column{
				"token",
				"char(32)",
				false,
				"PRI",
				"",
				""
			},
			DBPatcher::Column{
				"password",
				"char(32)",
				false,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"salt",
				"char(32)",
				false,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"email",
				"char(100)",
				false,
				"MUL",
				"",
				""
			},
			DBPatcher::Column{
				"date",
				"timestamp",
				false,
				"",
				"CURRENT_TIMESTAMP",
				"on update CURRENT_TIMESTAMP"
			},
		})
	};
	return TABLE_STRUCT;
}

}

#endif // NOVASTORY_DB_H