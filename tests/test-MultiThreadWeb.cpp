#include <QtTest>
#include <QTcpSocket>
#include "webserver/webserver.h"

class Test_MultiThreadWeb: public QObject
{
	Q_OBJECT
private slots:
	void basewebTest();
	void multithreadTest();
private:
	void mChecker();
signals:
	void webQuit();
};

void Test_MultiThreadWeb::multithreadTest()
{
	/*
	novastory::WebServer w;

	const int readersCount = 8;
	QTcpSocket htmlReader[readersCount]; // number of readers
	int threadCount = 0;
	int maxThread = 0;
	int threadLeft = readersCount;
	for(int i = 0; i < readersCount; ++i)
	{
		connect(&htmlReader[i], &QTcpSocket::connected, [&htmlReader, i, &threadCount, &maxThread](){
			qDebug() << "connect";
			threadCount++;
			maxThread = std::max(maxThread, threadCount);
			htmlReader[i].write("GET /index.html HTTP/1.1\nHost: www.example.com");
		});
		connect(&htmlReader[i], &QTcpSocket::disconnected, [this, &threadLeft, &maxThread](){
			qDebug() << "disconnect";
			threadLeft--;
			if(!threadLeft) // quit
			{
				qDebug() << "max thread: " << maxThread;
				QVERIFY(maxThread > 1);
				emit webQuit();
			}
		});
		connect(&htmlReader[i], &QTcpSocket::readyRead, [&htmlReader, i, &threadCount](){
			QByteArray html = htmlReader[i].readAll();
			QVERIFY(html.indexOf("<head>", Qt::CaseInsensitive) >= 0);
			threadCount--;
			htmlReader[i].close();
		});
	}
	for(int i = 0; i < readersCount; ++i)
		htmlReader[i].connectToHost("127.0.0.1", 8008);

	QEventLoop loop;
	connect(this, SIGNAL(webQuit()), &loop, SLOT(quit()));
	loop.exec();
	*/
}

void Test_MultiThreadWeb::basewebTest()
{
	novastory::WebServer w;

	QTcpSocket htmlReader;
	connect(&htmlReader, &QTcpSocket::connected, [&htmlReader](){
		qDebug() << "connect";
		htmlReader.write("GET /index.html HTTP/1.1\nHost: www.example.com");
	});
	connect(&htmlReader, &QTcpSocket::disconnected, [this](){
		qDebug() << "disconnect";
		emit webQuit();
	});
	connect(&htmlReader, &QTcpSocket::readyRead, [&htmlReader](){
		QByteArray html = htmlReader.readAll();
		QVERIFY(html.indexOf("<head>", Qt::CaseInsensitive) >= 0);
		htmlReader.close();
	});
	htmlReader.connectToHost("127.0.0.1", 8008);

	QEventLoop loop;
	connect(this, SIGNAL(webQuit()), &loop, SLOT(quit()));
	loop.exec();
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_MultiThreadWeb)
#include "test-MultiThreadWeb.moc"
