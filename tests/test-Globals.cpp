#include <QtTest>
#include "utils/globals.h"
#include <QFile>

using namespace novastory;

class Test_Globals: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void md5Test();
	void sha1Test();
	void unixtimeTest();
	void mailSendTest();
};

void Test_Globals::initTestCase()
{

}

void Test_Globals::init()
{
	
}

void Test_Globals::cleanup()
{
	
}

void Test_Globals::cleanupTestCase()
{

}

void Test_Globals::md5Test()
{
	QCOMPARE(md5("md5"), QString("1bc29b36f623ba82aaf6724fd3b16718"));
	QCOMPARE(md5("md4"), QString("c93d3bf7a7c4afe94b64e30c2ce39f4f"));
	QCOMPARE(md5(""), QString("d41d8cd98f00b204e9800998ecf8427e"));
}

void Test_Globals::sha1Test()
{
	QCOMPARE(sha1("The quick brown fox jumps over the lazy dog"), QString("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"));
	//QCOMPARE(sha1("В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!"), QString("9e32295f8225803bb6d5fdfcc0674616a4413c1b"));
	QCOMPARE(sha1("sha"), QString("d8f4590320e1343a915b6394170650a8f35d6926"));
}

void Test_Globals::unixtimeTest()
{
	QVERIFY(unixtime() > 0);
}

void Test_Globals::mailSendTest()
{
	//sendMail("degitx@gmail.com", "sdsad", "dasdasd");
}



/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Globals)
#include "test-Globals.moc"
