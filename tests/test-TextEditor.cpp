#include <QtTest>
#include "sql/sqldatabase.h"
#include "sql/sqlquery.h"
#include "utils/globals.h"
#include "logic/user.h"

using namespace novastory;

class Test_TextEditor: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();
private:
	SqlDatabase db;
};

void Test_TextEditor::initTestCase()
{
	QVERIFY(db.open());
}

void Test_TextEditor::init()
{
	
}

void Test_TextEditor::cleanup()
{
	
}

void Test_TextEditor::cleanupTestCase()
{

}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_TextEditor)
#include "test-TextEditor.moc"
