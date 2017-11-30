#include <QtTest>
#include "webserver/sql/sqldatabase.h"
#include "webserver/sql/sqlquery.h"
#include "webserver/sql/sqlizable.h"

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
	void espaceString();
	void inListInt();
	void inListStrings();
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
	novastory::SqlQuery query;
	QVERIFY(query.exec("DROP TABLE `testtable`"));
}

void Test_QMySqlSimple::connectTest()
{
	//QVERIFY(database->open());
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


void Test_QMySqlSimple::espaceString()
{
	QCOMPARE(novastory::Sqlizable::escapeString("ab"), QString("'ab'"));
	QCOMPARE(novastory::Sqlizable::escapeString("a'b"), QString("'a''b'"));
}

void Test_QMySqlSimple::inListInt()
{
	QCOMPARE(novastory::Sqlizable::inList(QList<int>()), QString("IN(0)"));
	QCOMPARE(novastory::Sqlizable::inList(QList<int>() << 1 << 2), QString("IN(1, 2)"));
}

void Test_QMySqlSimple::inListStrings()
{
	QCOMPARE(novastory::Sqlizable::inList(QList<QString>() << "one" << "two"), QString("IN('one', 'two')"));
	QCOMPARE(novastory::Sqlizable::inList(QList<QString>() << "on'e" << "two"), QString("IN('on''e', 'two')"));
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_QMySqlSimple)
#include "test-QMySqlSimple.moc"
