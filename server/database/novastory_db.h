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
		}),
		QHash<QString, QList<QString>>{}
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
		}),
		QHash<QString, QList<QString>>{}
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
		}),
		QHash<QString, QList<QString>>{}
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
				false,
				"MUL",
				"",
				""
			},
			DBPatcher::Column{
				"release",
				"int(10) unsigned",
				false,
				"",
				"0",
				""
			},
			DBPatcher::Column{
				"createdate",
				"timestamp",
				false,
				"",
				"CURRENT_TIMESTAMP",
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
			DBPatcher::Column{
				"mark",
				"varchar(64)",
				true,
				"",
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
			DBPatcher::Column{
				"right_key",
				"int(10)",
				true,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"parent_id",
				"int(10) unsigned",
				true,
				"MUL",
				"",
				""
			},
			DBPatcher::Column{
				"left_key",
				"int(10)",
				true,
				"",
				"",
				""
			},
			DBPatcher::Column{
				"type",
				"enum('OTHER','REVISION','BOOK','TEXT')",
				false,
				"MUL",
				"OTHER",
				""
			},
		}),
		QHash<QString, QList<QString>>{}
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
			DBPatcher::Column{
				"lastrevision",
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
		}),
		QHash<QString, QList<QString>>{{"userid", QList<QString>{"userid", }}, }
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
		}),
		QHash<QString, QList<QString>>{}
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
		}),
		QHash<QString, QList<QString>>{{"userid_2", QList<QString>{"userid", "targetid", }}, }
	};
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
		}),
		QHash<QString, QList<QString>>{}
	};
	TABLE_STRUCT << DBPatcher::Table
	{
		"books",
		QList<DBPatcher::Column>({
			DBPatcher::Column{
				"bookid",
				"int(10) unsigned",
				false,
				"PRI",
				"",
				"auto_increment"
			},
			DBPatcher::Column{
				"textid",
				"int(10) unsigned",
				false,
				"MUL",
				"",
				""
			},
			DBPatcher::Column{
				"description",
				"text",
				true,
				"",
				"",
				""
			},
		}),
		QHash<QString, QList<QString>>{{"uniq_book", QList<QString>{"textid", }}, }
	};

	return TABLE_STRUCT;
}

}

#endif // NOVASTORY_DB_H