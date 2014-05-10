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
	void loginByTokenTest();
	void syncTest();
	void confirmPasswordReset();
	void deleteUserTest();
	void validateUser();
	void subscribe();
private:
	int userid;
	QString salt;
};

void Test_LogicUsers::initTestCase()
{
	SqlQuery q;
	QVERIFY(q.exec("DELETE FROM users WHERE email = 'dasdasd@dasdasd.com'"));
	QVERIFY(q.exec("DELETE FROM users WHERE email = 'testmail@test.com'"));
	q.exec("DELETE FROM users WHERE email = 'sbuser1@noemail.com'");
	q.exec("DELETE FROM users WHERE email = 'sbuser2@noemail.com'");
	q.exec("DELETE FROM users WHERE email = 'sbuser3@noemail.com'");
}

void Test_LogicUsers::init()
{

}

void Test_LogicUsers::cleanup()
{
}

void Test_LogicUsers::cleanupTestCase()
{
	SqlQuery q;
	q.exec("DELETE FROM redmine.users WHERE mail = 'dasdasd@dasdasd.com'");
	q.exec("DELETE FROM redmine.users WHERE mail = 'testmail@test.com'");
}

void Test_LogicUsers::insetDataTest()
{
	User users;
	users.setRawPassword("dasdasdasd");
	QVERIFY(!users.addUser()); // no email, insert must be failed
	users.setEmail("dasdasd@dasdasd.com");
	QVERIFY(users.addUser());

	QVERIFY(users.userid() > 0); // from last insert

	userid = users.userid();
	salt = users.salt();
	QVERIFY(salt.size() > 0);
}

void Test_LogicUsers::userDuplicatinTest()
{
	User user2;
	user2.setRawPassword("saaaadasdsda");
	user2.setEmail("dasdasd@dasdasd.com");
	QVERIFY(!user2.addUser());
}

void Test_LogicUsers::userSyncTest()
{
	// trying primary_key methid
	User olduser1;
	olduser1.setUserID(userid);
	QVERIFY(olduser1.syncSQL("userid"));
	QCOMPARE(olduser1.email(), QString("dasdasd@dasdasd.com"));
	User olduser2;
	olduser2.setEmail("dasdasd@dasdasd.com");
	QVERIFY(olduser2.syncSQL("email"));
	QCOMPARE(olduser2.userid(), userid);
	QCOMPARE(olduser2.email(), QString("dasdasd@dasdasd.com"));
}

void Test_LogicUsers::loginTest()
{
	User user1;
	QVERIFY(!user1.isLogined());
	QVERIFY(!user1.login("dasdasd@dasdasd.com", sha1("dasdasdasa")));
	QVERIFY(user1.login("dasdasd@dasdasd.com", sha1("dasdasdasd")));
	QCOMPARE(user1.email(), QString("dasdasd@dasdasd.com"));
	QVERIFY(!user1.login("asdasd@dasdasd.com", sha1("dasdasdasd")));
	QVERIFY(user1.isLogined());
	QVERIFY(user1.token().length() > 0);
}


void Test_LogicUsers::loginByTokenTest()
{
	User user;
	QVERIFY(user.login("dasdasd@dasdasd.com", sha1("dasdasdasd")));
	QVERIFY(user.token().length() > 0);
	User same_user;
	QVERIFY(!same_user.loginByToken("dasdasd@dasdasd.com", "232323-dasdasdasdas"));
	QVERIFY(same_user.loginByToken("dasdasd@dasdasd.com", user.token()));
	User iduser;
	QVERIFY(iduser.loginByToken(userid, user.token()));
	QVERIFY(!iduser.loginByToken(3543543, user.token()));
}


void Test_LogicUsers::syncTest()
{
	User user1;
	user1.setUserID(userid);
	QVERIFY(user1.sync());
	QCOMPARE(user1.email(), QString("dasdasd@dasdasd.com"));
	User user2;
	user2.setEmail("dasdas@dasdasd.com");
	QVERIFY(!user2.sync());
	user2.setEmail("dasdasd@dasdasd.com");
	QVERIFY(user2.sync());
	QCOMPARE(user2.userid(), userid);
	QVERIFY(!user2.isLogined());
}


void Test_LogicUsers::confirmPasswordReset()
{
	QString newpass = "afkdhs";
	QString token = md5("tokentest");
	User vu;
	SqlQuery("INSERT INTO userspassforgot VALUES('" + token + "', '" + QString::number(userid) + "', '" + vu.generatePassword(sha1(newpass), salt) + "')");
	QVERIFY(!vu.confirmPasswordReset(md5("tokent")));
	QVERIFY(vu.confirmPasswordReset(token));
	User logintest;
	QVERIFY(!logintest.login("dasdasd@dasdasd.com", sha1("dasdasdasd")));
	QVERIFY(logintest.login("dasdasd@dasdasd.com", sha1(newpass)));

	// check cleanup
	SqlQuery q("SELECT * FROM userspassforgot WHERE token = '" + token + "'");
	QVERIFY(q.size() == 0);
}


void Test_LogicUsers::deleteUserTest()
{
	SqlQuery q;
	// Removing by hands test
	User olduser1;
	olduser1.setUserID(userid);
	QVERIFY(olduser1.syncSQL("userid"));
	q.exec("SELECT * FROM users WHERE email = 'dasdasd@dasdasd.com'");
	QCOMPARE(q.size(), 1);
	QVERIFY(olduser1.removeSQL("userid"));
	q.exec("SELECT * FROM users WHERE email = 'dasdasd@dasdasd.com'");
	QCOMPARE(q.size(), 0);
	QCOMPARE(olduser1.userid(), -1);
	QCOMPARE(olduser1.email(), QString());

	// Try again but wih removeUser() default
	User othernewuser;
	othernewuser.setRawPassword("dasdasdasd");
	othernewuser.setEmail("dohastdasdasd@dasdasd.com");
	QVERIFY(othernewuser.addUser());
	QVERIFY(othernewuser.removeUser());
	q.exec("SELECT * FROM users WHERE email = 'dohastdasdasd@dasdasd.com'");
	QCOMPARE(q.size(), 0);
	QCOMPARE(othernewuser.userid(), -1);
	QCOMPARE(othernewuser.email(), QString());

	// And again test data from another user
	othernewuser.setRawPassword("dasdasdasd");
	othernewuser.setEmail("dohastdasdasd@dasdasd.com");
	QVERIFY(!othernewuser.removeUser());
	QVERIFY(othernewuser.addUser());
	q.exec("SELECT * FROM users WHERE email = 'dohastdasdasd@dasdasd.com'");
	QCOMPARE(q.size(), 1);
	User othernewuser2;
	othernewuser2.setEmail("dohastdasdasd@dasdasd.com");
	QVERIFY(othernewuser2.removeUser());
	q.exec("SELECT * FROM users WHERE email = 'dohastdasdasd@dasdasd.com'");
	QCOMPARE(q.size(), 0);
}

void Test_LogicUsers::validateUser()
{
	SqlQuery q;
	QVERIFY(q.prepare("INSERT INTO usersverify(email, salt, password, token) VALUES(?,?,?,?)"));
	QVERIFY(User::verifyUser(md5("tokenfromgod")) == nullptr);
	q.bindValue(0, "testmail@test.com");
	q.bindValue(1, md5("saltfull"));
	q.bindValue(2, md5("coolpassword"));
	q.bindValue(3, md5("tokenfromgod"));
	QVERIFY(q.exec());
	QVERIFY(User::verifyUser("tokenfromgod") == nullptr);

	q.exec("SELECT * FROM users WHERE email = 'testmail@test.com'");
	QCOMPARE(q.size(), 0);
	q.exec("SELECT * FROM usersverify WHERE email = 'testmail@test.com'");
	QCOMPARE(q.size(), 1);

	// good
	QVERIFY(User::verifyUser(md5("tokenfromgod")) != nullptr);
	q.exec("SELECT * FROM users WHERE email = 'testmail@test.com'");
	QCOMPARE(q.size(), 1);
	q.exec("SELECT * FROM usersverify WHERE email = 'testmail@test.com'");
	QCOMPARE(q.size(), 0);

}

void Test_LogicUsers::subscribe()
{
	//new users for test
	User sbuser1;
	sbuser1.setEmail("sbuser1@noemail.com");
	sbuser1.setRawPassword("dogdog");
	QVERIFY(sbuser1.addUser());
	QVERIFY(sbuser1.userid() > 0);
	User sbuser2;
	sbuser2.setEmail("sbuser2@noemail.com");
	sbuser2.setRawPassword("dogdog");
	QVERIFY(sbuser2.addUser());
	User sbuser3;
	sbuser3.setEmail("sbuser3@noemail.com");
	sbuser3.setRawPassword("dogdog");
	QVERIFY(sbuser3.addUser());

	QVERIFY(!sbuser2.subscribe(sbuser1)); // cannot done without login
	QVERIFY(sbuser2.login("sbuser2@noemail.com", sha1("dogdog")));
	QVERIFY(sbuser3.login("sbuser3@noemail.com", sha1("dogdog")));

	QVERIFY(sbuser2.subscribe(sbuser1));
	QVERIFY(!sbuser2.subscribe(sbuser2)); // same user fail
	QVERIFY(sbuser3.subscribe(sbuser1));
	QVERIFY(!sbuser3.subscribe(sbuser1)); // dublicate

	SqlQuery test("SELECT * FROM subscriptions WHERE targetid = " + QString::number(sbuser1.userid()));
	QCOMPARE(test.size(), 2);

	QList<int> subscribtions = sbuser2.subscriptions();
	QCOMPARE(subscribtions.size(), 1);
	QCOMPARE(subscribtions[0], sbuser1.userid());

	QList<int> subscribed = sbuser1.subscribed();
	QCOMPARE(subscribed.size(), 2);
	QVERIFY(subscribed.contains(sbuser3.userid()));

	QCOMPARE(sbuser1.subscriptions().size(), 0);

	// cleanup
	QVERIFY(sbuser1.removeUser());
	QVERIFY(sbuser2.removeUser());
	QVERIFY(sbuser3.removeUser());
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_LogicUsers)
#include "test-LogicUsers.moc"
