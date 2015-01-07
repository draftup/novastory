#include <QtTest>
#include "webserver/logger.h"
#include <QFile>

using namespace novastory;

class Test_Logger: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void simpleOutputTest();
	void fileOutputTest();
};

void Test_Logger::initTestCase()
{
	QFile::remove("novastory.log");
}

void Test_Logger::init()
{

}

void Test_Logger::cleanup()
{

}

void Test_Logger::cleanupTestCase()
{
	QFile::remove("novastory.log");
}

void Test_Logger::simpleOutputTest()
{
	Logger::Instance().log("hahaha");
	qDebug() << "all good";
	QVERIFY(!QFile::exists("novastory.log"));
}

void Test_Logger::fileOutputTest()
{
	Logger::Instance().setWriteToLogFile(true);
	qDebug() << "all good 2";
	qDebug() << "all good 3";
	QVERIFY(QFile::exists("novastory.log"));

	QFile rd("novastory.log");
	QVERIFY(rd.open(QIODevice::ReadOnly | QIODevice::Text));
	QString r = rd.readAll();
	QVERIFY(r.indexOf("all good 2") != -1);
	rd.close();
}


/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Logger)
#include "test-Logger.moc"
