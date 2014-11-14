#include <QtTest>
#include "utils/globals.h"
#include "logic/bookinfo.h"
#include "logic/user.h"

using namespace novastory;

class Test_BookInfo: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void update();
	void sync();
private:
	User buser;
	BookInfo info;
	int bookid;
};

void Test_BookInfo::initTestCase()
{
	buser.setEmail("doentcare@dsadasd.ds");
	buser.setRawPassword("doentcare");
	QVERIFY(buser.addUser());
	QVERIFY(buser.login("doentcare@dsadasd.ds", sha1("doentcare")));
	info.setUser(buser);
}

void Test_BookInfo::init()
{

}

void Test_BookInfo::cleanup()
{

}

void Test_BookInfo::cleanupTestCase()
{
	QVERIFY(buser.removeUser());
	QVERIFY(info.remove());
}

void Test_BookInfo::update()
{
	info.setDescription("My Desc");
	info.setTextID(20);
	QVERIFY(info.update());
	QVERIFY(info.bookid() > 0);
	bookid = info.bookid();
	info.setTextID(21);
	QVERIFY(info.update());
}

void Test_BookInfo::sync()
{
	BookInfo s;
	s.setBookID(bookid);
	QVERIFY(s.sync());
	QCOMPARE(s.textid(), 21);
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_BookInfo)
#include "test-BookInfo.moc"
