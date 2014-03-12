#include <QtTest>
#include <QTcpSocket>
#include <QCoreApplication>
#include "rawfilehandler.h"

class Test_RawFileHandler: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void setDirectoryTest();
	void handleTest();
	void defaultHandleTest();
private:
	novastory::RawFileHandler* handler;
	QTcpSocket socket;
};

void Test_RawFileHandler::initTestCase()
{
}

void Test_RawFileHandler::init()
{
	handler = new novastory::RawFileHandler(&socket);
}

void Test_RawFileHandler::cleanup()
{
	delete handler;
}

void Test_RawFileHandler::cleanupTestCase()
{
}

void Test_RawFileHandler::setDirectoryTest()
{
	handler->setDirectory(QDir::currentPath());
	QString path = handler->directory();
	QVERIFY(path.size() > 0);
	handler->setDirectory(QDir::currentPath() + "/");
	QCOMPARE(handler->directory(), path);
	handler->setDirectory("");
	QCOMPARE(handler->directory(), path);
}

void Test_RawFileHandler::handleTest()
{
	handler->setDirectory(QCoreApplication::applicationDirPath());
	QVERIFY(handler->handle("GET", "/test-RawFileHandler.testfile"));
	QVERIFY(!handler->handle("GET", "/test-RawFileHandle.testfile"));
}

void Test_RawFileHandler::defaultHandleTest()
{
	QVERIFY(handler->handle("GET", "/images/main.png"));
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_RawFileHandler)
#include "test-RawFileHandler.moc"
