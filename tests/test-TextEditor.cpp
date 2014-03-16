#include <QtTest>
#include "sql/sqldatabase.h"
#include "sql/sqlquery.h"
#include "utils/globals.h"
#include "logic/texteditor.h"
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

	void testMain();
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
	User newuser;
	newuser.setEmail("doentcare@dsadasd.ds");
	newuser.setRawPassword("doentcare");
	newuser.removeUser();
}

void Test_TextEditor::testMain()
{
	TextEditor editor;
	QVERIFY(!editor.update());
	QVERIFY(editor.isJsonError());
	editor.jsonReset();
	QVERIFY(!editor.isJsonError());

	// Create test user
	User newuser;
	newuser.setEmail("doentcare@dsadasd.ds");
	newuser.setRawPassword("doentcare");
	QVERIFY(newuser.addUser());

	qDebug() << "userid = " << newuser.userid();
	editor.setUserID(newuser.userid());
	SqlQuery q;
	q.exec(QString("SELECT * FROM texteditor WHERE userid = ") + QString::number(newuser.userid()));
	QCOMPARE(q.size(), 0);
	QVERIFY(editor.update());
	q.exec(QString("SELECT * FROM texteditor WHERE userid = ") + QString::number(newuser.userid()));
	QCOMPARE(q.size(), 0);
	editor.setText("testtext");
	QVERIFY(editor.update());
	q.exec(QString("SELECT * FROM texteditor WHERE userid = ") + QString::number(newuser.userid()));
	QCOMPARE(q.size(), 1);
	editor.setText("testtext2");
	QVERIFY(editor.update());
	q.exec(QString("SELECT * FROM texteditor WHERE userid = ") + QString::number(newuser.userid()));
	QCOMPARE(q.size(), 1);
	editor.setText(QString());
	QVERIFY(editor.update());
	q.exec(QString("SELECT * FROM texteditor WHERE userid = ") + QString::number(newuser.userid()));
	QCOMPARE(q.size(), 0);

	// Cleanup
	QVERIFY(newuser.removeUser());
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_TextEditor)
#include "test-TextEditor.moc"
