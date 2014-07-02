#include <QtTest>
#include "sql/dbpatcher.h"
#include "sql/sqlquery.h"

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
	void modify();
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
	SqlQuery q("DROP TABLE test_table");
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
				"int unsigned",
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
}

void Test_DBPatcher::modify()
{
	patcher.m_database.clear();
	patcher.m_database << novastory::DBPatcher::Table
	{
		"test_table",
		QList<novastory::DBPatcher::Column>({
			novastory::DBPatcher::Column{
				"testid",
				"int unsigned",
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

	QVERIFY(patcher.patch());
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_DBPatcher)
#include "test-DBPatcher.moc"
