#include <QtTest>
#include "version.h"
#include "webserver/globals.h"

using namespace novastory;

class Test_Git: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();
private:
};

void Test_Git::initTestCase()
{
	QVERIFY(QString(GIT_REVISION).toInt() > 0);
}

void Test_Git::init()
{

}

void Test_Git::cleanup()
{

}

void Test_Git::cleanupTestCase()
{

}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Git)
#include "test-Git.moc"
