#include <QtTest>
#include <QSqlError>
#include "sql/dbpatcher.h"
#include "sql/sqlquery.h"
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

	SqlQuery q("select DATA_TYPE from information_schema.columns where table_schema = '" MYSQL_DATABASE "' AND TABLE_NAME = 'test_table' AND COLUMN_NAME = 'testfield'");
	QVERIFY(q.next());
	QCOMPARE(q.value("DATA_TYPE").toString(), QString("mediumtext"));
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_DBPatcher)
#include "test-DBPatcher.moc"
