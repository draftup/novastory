#include <QtTest>
#include "sql/sqldatabase.h"
#include "logic/users.h"

using namespace novastory;

class Test_LogicUsers: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void insetDataTest();
};

void Test_LogicUsers::initTestCase()
{

}

void Test_LogicUsers::init()
{
	
}

void Test_LogicUsers::cleanup()
{
	
}

void Test_LogicUsers::cleanupTestCase()
{

}

void Test_LogicUsers::insetDataTest()
{
	SqlDatabase db;
	QVERIFY(db.open());
	Users users;
	users.setUsername("testuser");
	users.setRawPassword("dasdasdasd");
	QVERIFY(!users.addUser()); // no email, insert must be failed
	users.setEmail("dasdasd@dasdasd.com");
	QVERIFY(users.addUser());

	QVERIFY(users.userid() > 0); // from last insert
}


/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_LogicUsers)
#include "test-LogicUsers.moc"
