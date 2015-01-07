#include <QtTest>
#include "webserver/sql/sqldatabase.h"
#include "webserver/sql/sqlquery.h"
#include "webserver/globals.h"
#include "logic/avatar.h"
#include "logic/user.h"

#include <QFile>
#include <QByteArray>

using namespace novastory;

class Test_Avatars: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void updateTest();
	void syncTest();
	void removeTest();
private:
	User newuser;
	Avatar avatar;
};

void Test_Avatars::initTestCase()
{
	newuser.setEmail("doentcare@dsadasd.ds");
	newuser.setRawPassword("doentcare");
	QVERIFY(newuser.addUser());
	QVERIFY(newuser.login("doentcare@dsadasd.ds", sha1("doentcare")));
}

void Test_Avatars::init()
{

}

void Test_Avatars::cleanup()
{

}

void Test_Avatars::cleanupTestCase()
{
	newuser.removeUser();
}


void Test_Avatars::updateTest()
{
	int uid = newuser.userid();
	Avatar a;

	QFile f("404.jpg");
	QVERIFY(f.open(QIODevice::ReadOnly));

	QByteArray bf = f.readAll();
	a.setAvatar(bf);
	f.close();

	a.setUserid(332543252); // no such user
	QVERIFY(!a.update());
	QCOMPARE(a.jsonErrorType(), 2);

	a.setUser(newuser);
	QVERIFY(a.update());
}

void Test_Avatars::syncTest()
{
	int uid = newuser.userid();
	QString email = newuser.email();

	Avatar a1;
	QCOMPARE(a1.modifyDate().toMSecsSinceEpoch(), 0);
	QVERIFY(!a1.sync());
	a1.setUserid(uid);
	QVERIFY(a1.sync());

	avatar.setEmail(email);
	QVERIFY(avatar.sync());

	QFile f("404.jpg");
	QVERIFY(f.open(QIODevice::ReadOnly));

	QByteArray bf = f.readAll();

	QVERIFY(avatar.avatar() == bf);
	QCOMPARE(avatar.contentSize(), (unsigned int)bf.size());
	QCOMPARE(avatar.contentType(), QString("image/jpeg"));
	QVERIFY(avatar.modifyDate().toMSecsSinceEpoch() > 200);
	f.close();
}

void Test_Avatars::removeTest()
{
	QString email = avatar.email();

	Avatar ra;
	ra.setEmail(email);
	QVERIFY(!ra.remove());
	ra.setUser(newuser);
	QVERIFY(ra.remove());

	SqlQuery q("SELECT * FROM avatars WHERE userid = " + QString::number(avatar.userid()));
	QCOMPARE(q.size(), 0);
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Avatars)
#include "test-Avatars.moc"
