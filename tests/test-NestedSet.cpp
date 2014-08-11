#include <QtTest>
#include "sql/sqldatabase.h"
#include "sql/sqlquery.h"
#include "utils/globals.h"

using namespace novastory;

class Test_NestedSet: public QObject
{
	Q_OBJECT
public:
	Test_NestedSet() {};
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();
private:
};

void Test_NestedSet::initTestCase()
{

}

void Test_NestedSet::init()
{

}

void Test_NestedSet::cleanup()
{

}

void Test_NestedSet::cleanupTestCase()
{

}



/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_NestedSet)
#include "test-NestedSet.moc"
