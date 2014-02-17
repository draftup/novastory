#include <QtTest>
#include "sql/sqldatabase.h"
#include "sql/sqlquery.h"
#include "utils/globals.h"
#include "logic/user.h"

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
	void userDuplicatinTest();
	void userSyncTest();
	void loginTest();
	void deleteUserTest();
private:
	int userid;
	SqlDatabase db;
};

void Test_LogicUsers::initTestCase()
{
	QVERIFY(db.open());
	SqlQuery q;
	QVERIFY(q.exec("DELETE FROM users WHERE username = 'testuser'"));
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
	User users;
	users.setUsername("testuser");
	users.setRawPassword("dasdasdasd");
	QVERIFY(!users.addUser()); // no email, insert must be failed
	users.setEmail("dasdasd@dasdasd.com");
	QVERIFY(users.addUser());

	QVERIFY(users.userid() > 0); // from last insert

	userid = users.userid();
}

void Test_LogicUsers::userDuplicatinTest()
{
	User user1;
	user1.setUsername("testuser");
	user1.setRawPassword("sadasdsda");
	user1.setEmail("other@dasdasd.com");
	User user2;
	user2.setUsername("asdaasdasd");
	user2.setRawPassword("saaaadasdsda");
	user2.setEmail("dasdasd@dasdasd.com");
	QVERIFY(!user1.addUser());
	QVERIFY(!user2.addUser());
}

void Test_LogicUsers::userSyncTest()
{
	// trying primary_key methid
	User olduser1;
	olduser1.setUserID(userid);
	QVERIFY(olduser1.syncSQL("userid"));
	QCOMPARE(olduser1.username(), QString("testuser"));
	QCOMPARE(olduser1.email(), QString("dasdasd@dasdasd.com"));
	User olduser2;
	olduser2.setUsername("testuser");
	QVERIFY(olduser2.syncSQL("username"));
	QCOMPARE(olduser2.userid(), userid);
	QCOMPARE(olduser2.email(), QString("dasdasd@dasdasd.com"));
	

}

void Test_LogicUsers::loginTest()
{
	User user1;
	QVERIFY(!user1.login("dasdasd@dasdasd.com", sha1("dasdasdasa")));
	QVERIFY(user1.login("dasdasd@dasdasd.com", sha1("dasdasdasd")));
	QCOMPARE(user1.username(), QString("testuser"));
	QVERIFY(!user1.login("asdasd@dasdasd.com", sha1("dasdasdasd")));
}


void Test_LogicUsers::deleteUserTest()
{
	User olduser1;
	olduser1.setUserID(userid);
	QVERIFY(olduser1.syncSQL("userid"));
	QVERIFY(olduser1.removeSQL("userid"));
	QCOMPARE(olduser1.userid(), -1);
	QCOMPARE(olduser1.username(), QString());
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_LogicUsers)
#include "test-LogicUsers.moc"
