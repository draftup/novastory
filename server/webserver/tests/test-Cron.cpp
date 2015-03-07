#include <QtTest>
#include "webserver/cron.h"
#include "webserver/webserver.h"
#include "webserver/sql/sqlquery.h"

using namespace novastory;

class Test_Cron: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void start();
};

void Test_Cron::initTestCase()
{

}

void Test_Cron::init()
{

}

void Test_Cron::cleanup()
{

}

void Test_Cron::cleanupTestCase()
{

}

void Test_Cron::start()
{

}


/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Cron)
#include "test-Cron.moc"
