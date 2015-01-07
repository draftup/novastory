#include <QtTest>
#include <QSqlError>
#include "webserver/sql/dbpatcher.h"
#include "webserver/sql/sqlquery.h"
#include "config.h"

using namespace novastory;

class Test_DBPatcher: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void create();
	void addtable();
	void addcolumn();
	void removecolumn();
	void modifycolumn();
	void modifyExtraColumn();
	void modifyKeys();
	void modifyUniKeys();

	void someWrongCases();
private:
	DBPatcher patcher;
};

void Test_DBPatcher::initTestCase()
{
	// clear db struct
	patcher.m_database.clear();
}

void Test_DBPatcher::init()
{

}

void Test_DBPatcher::cleanup()
{

}

void Test_DBPatcher::cleanupTestCase()
{
	SqlQuery("DROP TABLE test_table");
	SqlQuery("DROP TABLE test_table2");
}

void Test_DBPatcher::create()
{
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int(10) unsigned",
				false,
				"PRI"
			},
			novastory::DBPatcher::Column{
				"testfield",
				"text",
				true
			}
		})
	};

	QVERIFY(patcher.patch());

	SqlQuery q("SELECT * FROM test_table");
	QCOMPARE(q.lastError().type(), QSqlError::NoError);
}

void Test_DBPatcher::addtable()
{
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int(10) unsigned",
				false,
				"PRI"
			},
			novastory::DBPatcher::Column{
				"testfield",
				"text",
				true
			}
		})
	};
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table2",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testfield",
				"text",
				true
			}
		})
	};

	QVERIFY(patcher.patch());

	SqlQuery q("SELECT * FROM test_table2");
	QCOMPARE(q.lastError().type(), QSqlError::NoError);
}

void Test_DBPatcher::addcolumn()
{
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int(10) unsigned",
				false,
				"PRI"
			},
			novastory::DBPatcher::Column{
				"testfield",
				"text",
				true
			},
			novastory::DBPatcher::Column{
				"testfield2",
				"text",
				true
			},
		})
	};
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table2",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testfield",
				"text",
				true
			}
		})
	};

	QVERIFY(patcher.patch());

	SqlQuery q("SELECT testfield2 FROM test_table");
	QCOMPARE(q.lastError().type(), QSqlError::NoError);
}

void Test_DBPatcher::removecolumn()
{
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int(10) unsigned",
				false,
				"PRI"
			},
			novastory::DBPatcher::Column{
				"testfield",
				"text",
				true
			}
		})
	};
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table2",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testfield",
				"text",
				true
			}
		})
	};

	QVERIFY(patcher.patch());

	SqlQuery q("SELECT testfield2 FROM test_table");
	QVERIFY(q.lastError().type() != QSqlError::NoError);
}

void Test_DBPatcher::modifycolumn()
{
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int(10) unsigned",
				false,
				"PRI"
			},
			novastory::DBPatcher::Column{
				"testfield",
				"mediumtext",
				true
			},
			// add new extra field
			DBPatcher::Column{
				"somedate",
				"timestamp",
				true
			},
		})
	};
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table2",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testfield",
				"text",
				true
			}
		})
	};

	QVERIFY(patcher.patch());

	SqlQuery q("select DATA_TYPE from information_schema.columns where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table' AND COLUMN_NAME = 'testfield'");
	QVERIFY(q.next());
	QCOMPARE(q.value("DATA_TYPE").toString(), QString("mediumtext"));
	SqlQuery q2("select DATA_TYPE from information_schema.columns where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table' AND COLUMN_NAME = 'somedate'");
	QVERIFY(q2.next());
	QCOMPARE(q2.value("DATA_TYPE").toString(), QString("timestamp"));
}


void Test_DBPatcher::modifyExtraColumn()
{
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int(10) unsigned",
				false,
				"PRI"
			},
			novastory::DBPatcher::Column{
				"testfield",
				"mediumtext", // modify data
				true
			},
			// add new extra field
			DBPatcher::Column{
				"somedate",
				"timestamp",
				false,
				"",
				"CURRENT_TIMESTAMP",
				"on update CURRENT_TIMESTAMP"
			},
		})
	};
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table2",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testfield",
				"text",
				true
			}
		})
	};

	QVERIFY(patcher.patch());

	SqlQuery q("select COLUMN_DEFAULT, EXTRA from information_schema.columns where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table' AND COLUMN_NAME = 'somedate'");
	QVERIFY(q.next());
	QCOMPARE(q.value("COLUMN_DEFAULT").toString(), QString("CURRENT_TIMESTAMP"));
	QCOMPARE(q.value("EXTRA").toString(), QString("on update CURRENT_TIMESTAMP"));
}

void Test_DBPatcher::modifyKeys()
{
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int(10) unsigned",
				false,
				""
			},
			novastory::DBPatcher::Column{
				"testfield",
				"int(10) unsigned", // modify data
				true,
				"MUL"
			},
			// add new extra field
			DBPatcher::Column{
				"somedate",
				"timestamp",
				false,
				"",
				"CURRENT_TIMESTAMP",
				"on update CURRENT_TIMESTAMP"
			},
		})
	};
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table2",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testfield",
				"int(10) unsigned",
				true,
				"MUL"
			}
		})
	};
	SqlQuery("DROP TABLE test_table2");
	QVERIFY(patcher.patch());

	SqlQuery q("select COLUMN_KEY from information_schema.columns where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table' AND COLUMN_NAME = 'testfield'");
	QVERIFY(q.next());
	QCOMPARE(q.value("COLUMN_KEY").toString(), QString("MUL"));
	SqlQuery q2("select COLUMN_KEY from information_schema.columns where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table' AND COLUMN_NAME = 'testid'");
	QVERIFY(q2.next());
	QCOMPARE(q2.value("COLUMN_KEY").toString(), QString(""));
	SqlQuery q3("select COLUMN_KEY from information_schema.columns where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table2' AND COLUMN_NAME = 'testfield'");
	QVERIFY(q3.next());
	QCOMPARE(q3.value("COLUMN_KEY").toString(), QString("MUL"));

	// Возвращаем обратно
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int(10) unsigned",
				false,
				"PRI"
			},
			novastory::DBPatcher::Column{
				"testfield",
				"int(10) unsigned", // modify data
				true
			},
			// add new extra field
			DBPatcher::Column{
				"somedate",
				"timestamp",
				false,
				"",
				"CURRENT_TIMESTAMP",
				"on update CURRENT_TIMESTAMP"
			},
		})
	};
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table2",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testfield",
				"int(10) unsigned",
				true
			}
		})
	};

	QVERIFY(patcher.patch());

	SqlQuery q4("select COLUMN_KEY from information_schema.columns where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table' AND COLUMN_NAME = 'testid'");
	QVERIFY(q4.next());
	QCOMPARE(q4.value("COLUMN_KEY").toString(), QString("PRI"));
	SqlQuery q5("select COLUMN_KEY from information_schema.columns where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table' AND COLUMN_NAME = 'testfield'");
	QVERIFY(q5.next());
	QCOMPARE(q5.value("COLUMN_KEY").toString(), QString(""));
}


void Test_DBPatcher::modifyUniKeys()
{
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int(10) unsigned",
				false,
				""
			},
			novastory::DBPatcher::Column{
				"testfield",
				"int(10) unsigned", // modify data
				true,
				""
			},
			// add new extra field
			DBPatcher::Column{
				"somedate",
				"timestamp",
				false,
				"",
				"CURRENT_TIMESTAMP",
				"on update CURRENT_TIMESTAMP"
			},
		}),
		QHash<QString, QList<QString>>{{ "textx", QList<QString>{"testid", "testfield", } }, }
	};
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table2",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testfield",
				"int(10) unsigned",
				true,
				"MUL"
			}
		})
	};
	QVERIFY(patcher.patch());
	SqlQuery q4("select * from information_schema.key_column_usage where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table' AND CONSTRAINT_NAME = 'textx'");
	QCOMPARE(q4.size(), 2);

	// delete
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int(10) unsigned",
				false,
				""
			},
			novastory::DBPatcher::Column{
				"testfield",
				"int(10) unsigned", // modify data
				true,
				"MUL"
			},
			// add new extra field
			DBPatcher::Column{
				"somedate",
				"timestamp",
				false,
				"",
				"CURRENT_TIMESTAMP",
				"on update CURRENT_TIMESTAMP"
			},
		})
	};
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table2",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testfield",
				"int(10) unsigned",
				true,
				"MUL"
			}
		})
	};
	QVERIFY(patcher.patch());
	SqlQuery q5("select * from information_schema.key_column_usage where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table' AND CONSTRAINT_NAME = 'textx'");
	QCOMPARE(q5.size(), 0);
	SqlQuery q6("select COLUMN_KEY from information_schema.columns where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table' AND COLUMN_NAME = 'testfield'");
	QVERIFY(q6.next());
	QCOMPARE(q6.value("COLUMN_KEY").toString(), QString("MUL"));
}

void Test_DBPatcher::someWrongCases()
{
	// Неправильно изменяем тип
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int(10) unsigned",
				false,
				"PRI"
			},
			novastory::DBPatcher::Column{
				"testfield",
				"lolololol",
				true
			},
			// add new extra field
			DBPatcher::Column{
				"somedate",
				"timestamp",
				true
			},
		})
	};
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table2",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testfield",
				"text",
				true
			}
		})
	};

	QVERIFY(!patcher.patch());
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_DBPatcher)
#include "test-DBPatcher.moc"
