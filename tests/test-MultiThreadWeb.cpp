#include <QtTest>
#include <QTcpSocket>
#include "webserver/webserver.h"
#include "rawfilehandler.h"

class Test_MultiThreadWeb: public QObject
{
	Q_OBJECT
private slots:
	void setDirectoryTest();
	void basewebTest();
	void multifileTest();
private:
	void mChecker();
signals:
	void webQuit();
};

void Test_MultiThreadWeb::setDirectoryTest()
{
	novastory::WebServer::Instance(8007);
	novastory::WebServer::Instance().setDirectory(QDir::currentPath());
	QString path = novastory::WebServer::Instance().directory();
	QVERIFY(path.size() > 0);
	novastory::WebServer::Instance().setDirectory(QDir::currentPath() + "/");
	QCOMPARE(novastory::WebServer::Instance().directory(), path);
	novastory::WebServer::Instance().setDirectory("");
	QCOMPARE(novastory::WebServer::Instance().directory(), path);
	novastory::WebServer::Instance().resetDirectory();
}

void Test_MultiThreadWeb::multifileTest()
{
	QStringList testFiles;
	testFiles 
		<< "/images/editor.svg" 
		<< "/images/fbook.svg"
		<< "/images/gplus.svg"
		<< "/images/login.svg"
		<< "/images/message.svg"
		<< "/images/message-unread.svg"
		<< "/images/mylibrary.svg";

	const int readersCount = 7;
	QTcpSocket htmlReader[readersCount]; // number of readers
	int threadCount = 0;
	int maxThread = 0;
	int threadLeft = readersCount;
	for(int i = 0; i < readersCount; ++i)
	{
		connect(&htmlReader[i], &QTcpSocket::connected, [&htmlReader, i, &threadCount, &maxThread, &testFiles](){
			qDebug() << "connect";
			threadCount++;
			maxThread = std::max(maxThread, threadCount);
			htmlReader[i].write(("GET " + testFiles[i] + " HTTP/1.1\nHost: www.example.com").toLatin1());
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
		connect(&htmlReader[i], &QTcpSocket::readyRead, [&htmlReader, i, &threadCount, &testFiles](){
			QByteArray html = htmlReader[i].readAll();
			QString publicDirectory = novastory::WebServer::Instance().directory();
			qDebug() << "Public directory: " << publicDirectory;
			QFile file(publicDirectory + testFiles[i]);
			QVERIFY(file.open(QIODevice::ReadOnly));
			QByteArray fileData = file.readAll();
			QByteArray dataWithHeader;

			//adding header
			dataWithHeader = ("HTTP/1.1 200 OK\n"
				"Server: novastory\n"
				"Content-Type: image/svg+xml\n"
				"Content-Length: " + QString::number(fileData.size()) + "\n\n").toLatin1()
				+ fileData;
			
			QVERIFY(dataWithHeader == html);
			threadCount--;
			htmlReader[i].close();
		});
	}
	for(int i = 0; i < readersCount; ++i)
		htmlReader[i].connectToHost("127.0.0.1", novastory::WebServer::Instance().serverPort());

	QEventLoop loop;
	connect(this, SIGNAL(webQuit()), &loop, SLOT(quit()));
	loop.exec();
}

void Test_MultiThreadWeb::basewebTest()
{
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
	htmlReader.connectToHost("127.0.0.1", novastory::WebServer::Instance().serverPort());

	QEventLoop loop;
	connect(this, SIGNAL(webQuit()), &loop, SLOT(quit()));
	loop.exec();
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_MultiThreadWeb)
#include "test-MultiThreadWeb.moc"
