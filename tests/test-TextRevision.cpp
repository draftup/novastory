#include <QtTest>
#include "sql/sqldatabase.h"
#include "sql/sqlquery.h"
#include "utils/globals.h"
#include "logic/textrevisioncontainer.h"
#include "logic/user.h"

using namespace novastory;

class Test_TextRevision: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void createRevision();
	void syncRevision();
private:
	User buser;
	TextRevisionContainer container;
};

void Test_TextRevision::initTestCase()
{
	buser.setEmail("doentcar@dsadasd.ds");
	buser.setRawPassword("doentcare");
	QVERIFY(buser.addUser());
	QVERIFY(buser.login("doentcar@dsadasd.ds", sha1("doentcare")));
	container.setUser(buser);
}

void Test_TextRevision::init()
{

}

void Test_TextRevision::cleanup()
{

}

void Test_TextRevision::cleanupTestCase()
{
	SqlQuery q;
	q.exec(QString("SELECT * FROM textrevisions WHERE userid = ") + QString::number(buser.userid()));
	QCOMPARE(q.size(), 2);
	container.clear();
	q.exec(QString("SELECT * FROM textrevisions WHERE userid = ") + QString::number(buser.userid()));
	QCOMPARE(q.size(), 0);
	QCOMPARE(container.size(), 0);
	QVERIFY(buser.removeUser());
}

void Test_TextRevision::createRevision()
{
	QVERIFY(container.save("privet"));
	QCOMPARE(container.size(), 1);
	QVERIFY(container.save("privet2"));
	QCOMPARE(container.size(), 2);
}

void Test_TextRevision::syncRevision()
{
	TextRevisionContainer containerSync;
	containerSync.setUser(buser);
	QCOMPARE(containerSync.size(), 0);
	QVERIFY(containerSync.sync());
	QCOMPARE(containerSync.size(), 2);
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_TextRevision)
#include "test-TextRevision.moc"
