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
		m_table_name = "nested_table";
		m_left_name = "leftkey";
		m_right_name = "rightkey";
		m_data_name = "data";
		m_id_name = "id";
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

	void insert();
private:
	NestedSetTest m_set;
};

void Test_NestedSet::initTestCase()
{
	//SqlQuery("CREATE TABLE nested_table(id int unsigned auto_increment, primary key(id), leftkey int unsigned, rightkey int unsigned, data int)");
}

void Test_NestedSet::init()
{

}

void Test_NestedSet::cleanup()
{

}

void Test_NestedSet::cleanupTestCase()
{
	//SqlQuery("DROP TABLE nested_table");
}

void Test_NestedSet::insert()
{
	int root = m_set.insert(0, 2);
	qDebug() << "Root id = " << root;

	QVERIFY(root > 0);
	// Cheking root keys
	QCOMPARE(m_set.leftKey(root), 1);
	QCOMPARE(m_set.rightKey(root), 2);

	// вставляем лист
	QVERIFY(m_set.insert(root + 1, 2) < 0);
	int root_leef1 = m_set.insert(root, 3);
	QVERIFY(root_leef1 > 0);
	QCOMPARE(m_set.leftKey(root), 1);
	QCOMPARE(m_set.rightKey(root), 4);
	QCOMPARE(m_set.leftKey(root_leef1), 2);
	QCOMPARE(m_set.rightKey(root_leef1), 3);

	// вставляем соседний лист
	int root_leef2 = m_set.insert(root, 4);
	QVERIFY(root_leef2 > 0);
	QCOMPARE(m_set.leftKey(root), 1);
	QCOMPARE(m_set.rightKey(root), 6);
	QCOMPARE(m_set.leftKey(root_leef1), 2);
	QCOMPARE(m_set.rightKey(root_leef1), 3);
	QCOMPARE(m_set.leftKey(root_leef2), 4);
	QCOMPARE(m_set.rightKey(root_leef2), 5);
}



/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_NestedSet)
#include "test-NestedSet.moc"
