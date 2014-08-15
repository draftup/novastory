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
public:
	Test_TextRevision() : releaseID(-1) {};
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void unloginedTest();
	void createRevision();
	void updateRevision();
	void syncRevision();

	void releaseMiddle();
	void releaseLast();

	void unrelease();

	void dublicateCheck();
private:
	User buser;
	TextRevisionContainer container;
	int releaseID;
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
	QCOMPARE(q.size(), 5);
	container.clear();
	q.exec(QString("SELECT * FROM textrevisions WHERE userid = ") + QString::number(buser.userid()));
	QCOMPARE(q.size(), 0);
	QCOMPARE(container.size(), 0);
	QVERIFY(buser.removeUser());
}

void Test_TextRevision::unloginedTest()
{
	TextRevisionContainer c;
	QVERIFY(!c.insert("lol").isValid());
}

void Test_TextRevision::createRevision()
{
	QCOMPARE(container.size(), 0);
	QVERIFY(container.update("privet").isValid());
	QCOMPARE(container.size(), 1);
	QVERIFY(container.insert("privet2").isValid());
	QCOMPARE(container.size(), 2);
}


void Test_TextRevision::updateRevision()
{
	QVERIFY(container.update("privet22").isValid());
	QCOMPARE(container.size(), 2);
	TextRevision f = container.first();
	TextRevision l = container.last();
	QCOMPARE(f.text(), QString("privet"));
	QCOMPARE(l.text(), QString("privet22"));
}

void Test_TextRevision::syncRevision()
{
	TextRevisionContainer containerSync;
	containerSync.setUser(buser);
	QCOMPARE(containerSync.size(), 0);
	QVERIFY(containerSync.sync());
	QCOMPARE(containerSync.size(), 2);
}

void Test_TextRevision::releaseMiddle()
{
	QVERIFY(!container.release(0));
	QCOMPARE(container.first().isRelease(), false);
	QVERIFY(container.release(container.firstKey()));
	QCOMPARE(container.size(), 3);
	TextRevision& rv = container.last();
	QCOMPARE(rv.isRelease(), true);
	releaseID = rv.revisionId();
	container.release(rv.revisionId());
	QCOMPARE(container.size(), 3);
	QCOMPARE(rv, container.last());
}

void Test_TextRevision::releaseLast()
{
	QVERIFY(container.insert("privet4").isValid());
	QCOMPARE(container.size(), 4);
	TextRevision& rv = container.last();

	QCOMPARE(rv, container.revision(rv.revisionId()));

	QCOMPARE(rv.isRelease(), false);
	QCOMPARE(rv.text(), QString("privet4"));
	QVERIFY(container.release(rv));
	QCOMPARE(rv.isRelease(), true);
	QCOMPARE(container.size(), 4);

	// try to update based on new revision
	QVERIFY(container.update("privet5").isValid());
	QCOMPARE(container.size(), 5);
}


void Test_TextRevision::unrelease()
{
	QVERIFY(releaseID > 0);
	TextRevision rev = container.revision(releaseID);
	QCOMPARE(rev.isRelease(), true);
	QVERIFY(container.unrelease(rev));
	QCOMPARE(container.revision(releaseID).isRelease(), false);
}

void Test_TextRevision::dublicateCheck()
{
	QCOMPARE(container.size(), 5);
	QVERIFY(!container.insert("privet5").isValid());
	QCOMPARE(container.jsonErrorType(), 3);
	QCOMPARE(container.size(), 5);
	QVERIFY(!container.update("privet5").isValid());
	QCOMPARE(container.jsonErrorType(), 3);
}



/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_TextRevision)
#include "test-TextRevision.moc"
