#include <QtTest>
#include <QTcpSocket>
#include "webserver.h"

class Test_MultiThreadWeb: public QObject
{
	Q_OBJECT
private slots:
	void multithreadTest();
private:
};

void Test_MultiThreadWeb::multithreadTest()
{
	novastory::WebServer w;

	QTcpSocket htmlReader;
	connect(&htmlReader, &QTcpSocket::connected, [&htmlReader](){
		qDebug() << "connected";
	});
	connect(&htmlReader, &QTcpSocket::disconnected, [&htmlReader](){
		qDebug() << "disconnected";
	});
	connect(&htmlReader, &QTcpSocket::readyRead, [&htmlReader](){
		QByteArray html = htmlReader.readAll();
		qDebug() << html;
	});
	htmlReader.connectToHost("127.0.0.1", 8008);

	QEventLoop loop;
	loop.exec();
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_MultiThreadWeb)
#include "test-MultiThreadWeb.moc"
