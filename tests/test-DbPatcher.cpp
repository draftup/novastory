#include <QtTest>
#include "sql/dbpatcher.h"

using namespace novastory;

class Test_DBPatcher: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void patch();
private:
	DBPatcher patcher;
};

void Test_DBPatcher::initTestCase()
{
	// clear db struct
	patcher.m_database.clear();

	patcher.m_database << novastory::DBPatcher::Table{ 
		"test_table", 
		QList<novastory::DBPatcher::Column>({ 
			novastory::DBPatcher::Column{
				"testid",
				"unsigned int",
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
}

void Test_DBPatcher::init()
{

}

void Test_DBPatcher::cleanup()
{

}

void Test_DBPatcher::cleanupTestCase()
{

}

void Test_DBPatcher::patch()
{
	QVERIFY(patcher.patch());
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_DBPatcher)
#include "test-DBPatcher.moc"
