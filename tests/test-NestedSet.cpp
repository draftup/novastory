#include <QtTest>
#include "sql/sqldatabase.h"
#include "sql/sqlquery.h"
#include "utils/globals.h"
#include "utils/nestedset.h"

using namespace novastory;

class NestedSetTest : public NestedSet
{
public:
	NestedSetTest()
	{
		m_table_name = ""
	};
};

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
