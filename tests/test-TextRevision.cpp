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
	void createProject();
	void treeFolders();
	void updateRevision();
	void syncRevision();

	void releaseTest();
	void getRevision();

	void unrelease();
	void updateMark();

	void dublicateCheck();
	void removeCheck();

	void moveRevisions();
private:
	User buser;
	TextRevisionContainer container;
	int releaseID;
	int projectid;
};

void Test_TextRevision::initTestCase()
{
	buser.setEmail("doentcar@dsadasd.ds");
	buser.setRawPassword("doentcare");
	buser.removeUser();
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
	QCOMPARE(q.size(), 7);
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
	QCOMPARE(container.size(), 1); // only text
	QVERIFY(container.insert("privet2").isValid());
	QCOMPARE(container.size(), 2); // only text
	// also must created 2 folders for each revisions
	QCOMPARE(container.notLeefs().size(), 2);
}


void Test_TextRevision::createProject()
{
	TextRevision rev = container.newProject("privproject");
	QVERIFY(rev.isValid());
	projectid = rev.revisionId();
	QVERIFY(projectid > 0);
}


void Test_TextRevision::treeFolders()
{
	QVERIFY(container.treeFolders().contains("New Text"));
	QVERIFY(container.treeFolders().contains("privproject"));
}


void Test_TextRevision::updateRevision()
{
	QVERIFY(container.update("privet22").isValid()); // update last revisions
	QCOMPARE(container.size(), 2);
	TextRevision f = container.first();
	TextRevision l = container.last();
	QCOMPARE(f.text(), QString("privet"));
	QCOMPARE(l.text(), QString("privet22"));
	QVERIFY(container.update(f, "privet33").isValid());
	f = container.first();
	QCOMPARE(f.text(), QString("privet33"));
	QCOMPARE(l.text(), QString("privet22"));
}

void Test_TextRevision::syncRevision()
{
	TextRevisionContainer containerSync;
	containerSync.setUser(buser);
	QCOMPARE(containerSync.size(), 0);
	QVERIFY(containerSync.sync());
	QCOMPARE(containerSync.size(), 2);
	// sync folder
	TextRevisionContainer containerSync2;
	containerSync2.setUser(buser);
	QVERIFY(containerSync2.sync(containerSync.first().parent()));
	QCOMPARE(containerSync2.size(), 1);
}


void Test_TextRevision::releaseTest()
{
	QVERIFY(!container.release(0));
	QCOMPARE(container.last().isRelease(), false);
	QVERIFY(container.release(container.lastKey()));
	QCOMPARE(container.size(), 2);
	TextRevision& rv = container.last();
	QCOMPARE(rv.isRelease(), true);
	releaseID = rv.revisionId();
	container.release(rv.revisionId());
	QCOMPARE(container.size(), 2);

	// check parent release now
	TextRevision parentRev = container.revision(rv.parent());
	QVERIFY(parentRev.isValid());
	QCOMPARE(parentRev.isRelease(), true);

	QCOMPARE(container.size(), 2);

	// try to update based on new revision
	QVERIFY(container.update("privet5").isValid());
	QCOMPARE(container.size(), 3);
}

void Test_TextRevision::getRevision()
{
	TextRevision relRev = *(container.end() - 2);
	QCOMPARE(relRev.isRelease(), true);
	TextRevisionContainer c;
	// we havent access to unrealsed revision from other users
	QVERIFY(c.revision(relRev.revisionId()).isValid());
}


void Test_TextRevision::unrelease()
{
	QVERIFY(releaseID > 0);
	TextRevision rev = container.revision(releaseID);
	QCOMPARE(rev.isRelease(), true);
	QVERIFY(container.unrelease(rev));
	QCOMPARE(container.revision(releaseID).isRelease(), false);
}


void Test_TextRevision::updateMark()
{
	container.setMark("Lola");
	QVERIFY(container.updateMark(container.last()));
	QCOMPARE(container.last().mark(), QString("Lola"));
}

void Test_TextRevision::dublicateCheck()
{
	QCOMPARE(container.size(), 3);
	QVERIFY(container.insert("privet5").isValid());
	QCOMPARE(container.size(), 4); // all ok because of we insert absolute new text
	QVERIFY(!container.update("privet5").isValid());
	QCOMPARE(container.jsonErrorType(), 3);
}

void Test_TextRevision::removeCheck()
{
	QCOMPARE(container.size(), 4);
	QVERIFY(container.removeRevision(container.last()));
	QCOMPARE(container.size(), 3);
}

void Test_TextRevision::moveRevisions()
{
	QCOMPARE(container.size(), 3);
	TextRevision f = container.first();
	TextRevision l = container.last();
	QCOMPARE(l.text(), QString("privet5"));
	QCOMPARE(f.text(), QString("privet33"));
	QVERIFY(!container.move(l, f)); // rev into rev
	QVERIFY(!container.move(f, l)); // rev into rev
	QVERIFY(!container.move(l.revisionId(), projectid)); // rev into other
	QVERIFY(!container.move(l.parent(), f.revisionId())); // text into rev
	QVERIFY(container.move(l.parent(), projectid)); // text into other - ok
	QVERIFY(container.move(f.parent(), projectid)); // text into other - ok
	QVERIFY(!container.move(f.parent(), projectid)); // alredy moved
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_TextRevision)
#include "test-TextRevision.moc"
