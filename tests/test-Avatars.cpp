#include <QtTest>
#include "sql/sqldatabase.h"
#include "sql/sqlquery.h"
#include "utils/globals.h"
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
private:
	SqlDatabase db;
	User newuser;
};

void Test_Avatars::initTestCase()
{
	QVERIFY(db.open());

	newuser.setEmail("doentcare@dsadasd.ds");
	newuser.setRawPassword("doentcare");
	QVERIFY(newuser.addUser());
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
	a.setUserid(uid);
	a.setAvatar(bf);
	f.close();

	QVERIFY(a.update());
}

void Test_Avatars::syncTest()
{
	int uid = newuser.userid();
	QString email = newuser.email();

	Avatar a1;
	QVERIFY(!a1.sync());
	a1.setUserid(uid);
	QVERIFY(a1.sync());

	Avatar a2;
	a2.setEmail(email);
	QVERIFY(a2.sync());

	QFile f("404.jpg");
	QVERIFY(f.open(QIODevice::ReadOnly));

	QByteArray bf = f.readAll();

	QVERIFY(a2.avatar() == bf);
	f.close();
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Avatars)
#include "test-Avatars.moc"
