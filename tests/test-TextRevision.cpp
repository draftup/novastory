#include <QtTest>
#include "sql/sqldatabase.h"
#include "sql/sqlquery.h"
#include "utils/globals.h"
#include "logic/textrevision.h"
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

	void testMain();
private:
};

void Test_TextRevision::initTestCase()
{

}

void Test_TextRevision::init()
{

}

void Test_TextRevision::cleanup()
{

}

void Test_TextRevision::cleanupTestCase()
{

}

void Test_TextRevision::testMain()
{

}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_TextRevision)
#include "test-TextRevision.moc"
