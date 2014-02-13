#include <QtTest>
#include "sql/sqldatabase.h"
#include "sql/sqlquery.h"

class Test_QMySqlSimple: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void connectTest();
	void queryTest();
	void dataTest();
private:
	novastory::SqlDatabase* database;
};

void Test_QMySqlSimple::initTestCase()
{
	QVERIFY(novastory::SqlDatabase::isDriverAvailable("QMYSQL"));
	database = new novastory::SqlDatabase;
}

void Test_QMySqlSimple::init()
{
	
}

void Test_QMySqlSimple::cleanup()
{
	
}

void Test_QMySqlSimple::cleanupTestCase()
{
	novastory::SqlQuery query;
	QVERIFY(query.exec("DROP TABLE `testtable`"));
	delete database;
}

void Test_QMySqlSimple::connectTest()
{
	QVERIFY(database->open());
}

void Test_QMySqlSimple::queryTest()
{
	novastory::SqlQuery query;
	QVERIFY(query.exec("CREATE TABLE `testtable` (`test` varchar(100) NOT NULL DEFAULT '')"));
	QVERIFY(query.exec("INSERT INTO `testtable` VALUES('testx')"));
	query.prepare("INSERT INTO `testtable` VALUES(?)");
	query.bindValue(0, "testy");
	query.exec();
}

void Test_QMySqlSimple::dataTest()
{
	novastory::SqlQuery query("SELECT * FROM `testtable`;");
	QVERIFY(query.next());
	QCOMPARE(query.value(0).toString(), QString("testx"));
	QVERIFY(query.next());
	QCOMPARE(query.value(0).toString(), QString("testy"));
}


/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_QMySqlSimple)
#include "test-QMySqlSimple.moc"
