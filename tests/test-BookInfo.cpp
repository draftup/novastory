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
private:
	User buser;
};

void Test_BookInfo::initTestCase()
{
	buser.setEmail("doentcare@dsadasd.ds");
	buser.setRawPassword("doentcare");
	QVERIFY(buser.addUser());
	QVERIFY(buser.login("doentcare@dsadasd.ds", sha1("doentcare")));
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
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_BookInfo)
#include "test-BookInfo.moc"
