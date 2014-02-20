#include <QtTest>
#include "jsonthrower.h"

using namespace novastory;

class Test_JsonThrower: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void throwErrorTest();
	void resetTest();
	void insertTest();
private:
	JsonThrower thrower;
};

void Test_JsonThrower::initTestCase()
{

}

void Test_JsonThrower::init()
{
	
}

void Test_JsonThrower::cleanup()
{
	
}

void Test_JsonThrower::cleanupTestCase()
{

}

void Test_JsonThrower::throwErrorTest()
{
	QVERIFY(thrower.jsonString().indexOf("blabla") == -1);
	thrower.JSON_ERROR("blabla", 2);
	QVERIFY(thrower.jsonString().indexOf("blabla") != -1);
	QVERIFY(thrower.jsonString().indexOf("2") != -1);
}

void Test_JsonThrower::resetTest()
{
	thrower.jsonReset();
	QVERIFY(thrower.jsonString().indexOf("blabla") == -1);
}

void Test_JsonThrower::insertTest()
{
	thrower.JSON_ERROR("blabla", 2);
	thrower.JSON_INSERT("realcoolkey", QJsonValue(QString("bubble")));
	QVERIFY(thrower.jsonString().indexOf("blabla") != -1);
	QVERIFY(thrower.jsonString().indexOf("bubble") != -1);
	QVERIFY(thrower.jsonString().indexOf("realcoolkey") != -1);
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_JsonThrower)
#include "test-JsonThrower.moc"
