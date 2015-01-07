#include <QtTest>
#include "webserver/sql/sqldatabase.h"
#include "webserver/sql/sqlquery.h"
#include "webserver/globals.h"
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
	User buser;
};

void Test_TextEditor::initTestCase()
{
	buser.setEmail("doentcare@dsadasd.ds");
	buser.setRawPassword("doentcare");
	QVERIFY(buser.addUser());
	QVERIFY(buser.login("doentcare@dsadasd.ds", sha1("doentcare")));
}

void Test_TextEditor::init()
{

}

void Test_TextEditor::cleanup()
{

}

void Test_TextEditor::cleanupTestCase()
{
	QVERIFY(buser.removeUser());
}

void Test_TextEditor::testMain()
{
	TextEditor editor;
	QVERIFY(!editor.update());
	QVERIFY(editor.isJsonError());
	editor.jsonReset();
	QVERIFY(!editor.isJsonError());

	editor.setUser(buser);
	SqlQuery q;
	q.exec(QString("SELECT * FROM texteditor WHERE userid = ") + QString::number(buser.userid()));
	QCOMPARE(q.size(), 0);
	QVERIFY(editor.update());
	q.exec(QString("SELECT * FROM texteditor WHERE userid = ") + QString::number(buser.userid()));
	QCOMPARE(q.size(), 0);
	editor.setText("testtext");
	QVERIFY(editor.update());
	q.exec(QString("SELECT * FROM texteditor WHERE userid = ") + QString::number(buser.userid()));
	QCOMPARE(q.size(), 1);
	editor.setText("testtext2");
	QVERIFY(editor.update());
	q.exec(QString("SELECT * FROM texteditor WHERE userid = ") + QString::number(buser.userid()));
	QCOMPARE(q.size(), 1);
	editor.setText(QString());
	QVERIFY(editor.update());
	q.exec(QString("SELECT * FROM texteditor WHERE userid = ") + QString::number(buser.userid()));
	QCOMPARE(q.size(), 0);
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_TextEditor)
#include "test-TextEditor.moc"
