#include <QtTest>
#include <QTcpSocket>
#include <QCoreApplication>
#include "rawfilehandler.h"
#include "webserver/webserver.h"

using namespace novastory;

class Test_RawFileHandler: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

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

void Test_RawFileHandler::handleTest()
{
	WebServer::Instance().setDirectory(QCoreApplication::applicationDirPath());
	QVERIFY(handler->handle("GET", "/test-RawFileHandler.testfile"));
	QVERIFY(!handler->handle("GET", "/test-RawFileHandle.testfile"));
}

void Test_RawFileHandler::defaultHandleTest()
{
	WebServer::Instance().resetDirectory();
	QVERIFY(handler->handle("GET", "/images/editor.svg"));
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_RawFileHandler)
#include "test-RawFileHandler.moc"
