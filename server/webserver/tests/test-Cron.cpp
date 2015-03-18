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
	Cron::addFunc("testtask", [](int taskid, const QString & args, bool last_call)
	{
		QCOMPARE(args, QString("200"));
		qDebug() << "shot";
	});

	int id = Cron::startTask("testtask", "200", 200, false, QDateTime::fromMSecsSinceEpoch(QDateTime::currentDateTime().toMSecsSinceEpoch() + 1000));
	SqlQuery tsk("SELECT * FROM cron WHERE taskid = " + QString::number(id));
	QCOMPARE(tsk.size(), 1);
	QTest::qWait(1500);
	SqlQuery tsk2("SELECT * FROM cron WHERE taskid = " + QString::number(id));
	QCOMPARE(tsk2.size(), 0);
}


/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Cron)
#include "test-Cron.moc"
