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
private:
	int userid;
	QString salt;
};

void Test_LogicUsers::initTestCase()
{
	SqlQuery q;
	QVERIFY(q.exec("DELETE FROM users WHERE email = 'dasdasd@dasdasd.com'"));
	QVERIFY(q.exec("DELETE FROM users WHERE email = 'testmail@test.com'"));
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

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_LogicUsers)
#include "test-LogicUsers.moc"
