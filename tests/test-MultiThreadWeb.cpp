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

	const int SERVERS = 8;
	QTcpSocket htmlReader[SERVERS];
	for(int i = 0; i < SERVERS; ++i)
	{
		connect(&htmlReader[i], &QTcpSocket::connected, [&htmlReader, i](){
			qDebug() << "connect " << i;
			htmlReader[i].write("GET /index.html HTTP/1.1\nHost: www.example.com");
		});
		connect(&htmlReader[i], &QTcpSocket::disconnected, [this, i](){
			qDebug() << "disconnect " << i;
			emit webQuit();
		});
		connect(&htmlReader[i], &QTcpSocket::readyRead, [&htmlReader, i](){
			QByteArray html = htmlReader[i].readAll();
			QVERIFY(html.indexOf("HTTP/1.0 200", Qt::CaseInsensitive) >= 0);
			qDebug() << "readed " << i;
			htmlReader[i].close();
		});
		htmlReader[i].connectToHost("127.0.0.1", 8008);

		QEventLoop loop;
		connect(this, SIGNAL(webQuit()), &loop, SLOT(quit()));
		loop.exec();
	}
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
		QVERIFY(html.indexOf("HTTP/1.0 200", Qt::CaseInsensitive) >= 0);
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
