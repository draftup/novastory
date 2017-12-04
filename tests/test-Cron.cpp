#include <QtTest>
#include <QSqlError>
#include "webserver/cron.h"
#include "webserver/webserver.h"
#include "webserver/sql/sqlquery.h"
#include "webserver/sql/dbpatcher.h"

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
	void stop();
	void change();
private:
	DBPatcher patcher;
};

void Test_Cron::initTestCase()
{
	SqlQuery checkTable("SELECT 1 FROM cron LIMIT 1");
	if (checkTable.lastError().type() != QSqlError::NoError)
	{
		qDebug() << "Recreate table";
		QSet<novastory::DBPatcher::Table> m_database;
		m_database << DBPatcher::Table
		{
			"cron",
			QList<DBPatcher::Column>({
			DBPatcher::Column{
			"taskid",
			"int(10) unsigned",
			false,
			"PRI",
			"",
			"auto_increment"
		},
				DBPatcher::Column{
			"task",
			"char(128)",
			false,
			"",
			"",
			""
		},
				DBPatcher::Column{
			"args",
			"varchar(255)",
			true,
			"",
			"",
			""
		},
				DBPatcher::Column{
			"starttime",
			"timestamp",
			false,
			"",
			"CURRENT_TIMESTAMP",
			""
		},
				DBPatcher::Column{
			"endtime",
			"timestamp",
			true,
			"",
			"",
			""
		},
				DBPatcher::Column{
			"oncetime",
			"tinyint(1)",
			false,
			"",
			"",
			""
		},
				DBPatcher::Column{
			"interval",
			"int(11) unsigned",
			true,
			"",
			"",
			""
		},
		}),
			QHash < QString, QList<QString> > {}
		};
		patcher.setDatabaseStructure(m_database);
		QVERIFY(patcher.patch());
	}
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

void Test_Cron::stop()
{
	int id = Cron::startTask("testtask", "200", 200, true);
	Cron::stopTask(id);
	QTest::qWait(250);
	SqlQuery tsk2("SELECT * FROM cron WHERE taskid = " + QString::number(id));
	QCOMPARE(tsk2.size(), 0);
}


void Test_Cron::change()
{
	int id = Cron::startTask("testtask", "200", 200, true);
	Cron::setTaskTime(id, QDateTime::fromMSecsSinceEpoch(QDateTime::currentDateTime().toMSecsSinceEpoch() + 500));
	QTest::qWait(250);
	SqlQuery tsk2("SELECT * FROM cron WHERE taskid = " + QString::number(id));
	QCOMPARE(tsk2.size(), 1);
	QTest::qWait(300);
	SqlQuery tsk3("SELECT * FROM cron WHERE taskid = " + QString::number(id));
	QCOMPARE(tsk3.size(), 0);
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Cron)
#include "test-Cron.moc"
