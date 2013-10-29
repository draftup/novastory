#include <QtTest>
#include "sql/sqldatabase.h"

class Test_QMySqlSimple: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();
private:
};

void Test_QMySqlSimple::initTestCase()
{
	QVERIFY(novastory::SqlDatabase::isDriverAvailable("QMYSQL"));
}

void Test_QMySqlSimple::init()
{
}

void Test_QMySqlSimple::cleanup()
{
}

void Test_QMySqlSimple::cleanupTestCase()
{
}


/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_QMySqlSimple)
#include "test-QMySqlSimple.moc"
