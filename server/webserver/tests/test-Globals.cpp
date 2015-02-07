#include <QtTest>
#include "webserver/globals.h"
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
	void RFC822DateTest();
	void selectorTest();
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

void Test_Globals::RFC822DateTest()
{
	QCOMPARE(RFC822Date(QDateTime::fromMSecsSinceEpoch(0)), QString("Thu, 01 Jan 1970 00:00:00 GMT"));
}

void Test_Globals::selectorTest()
{
	QCOMPARE(selectorId(QString(), "dg"), QString());
	QCOMPARE(selectorId("aa<div id=\"deg\">dsds</div>bb", "dg"), QString("aa<div id=\"deg\">dsds</div>bb"));

	// basic select
	QCOMPARE(selectorId("aa<div id=\"deg\">dsds</div>bb", "deg"), QString("<div id=\"deg\">dsds</div>"));
	QCOMPARE(selectorId("aa<div id='deg'>dsds</div>bb", "deg"), QString("<div id='deg'>dsds</div>"));
	QCOMPARE(selectorId("aa<p id='deg'></p>bb", "deg"), QString("<p id='deg'></p>"));

	// more hard tests
	QCOMPARE(selectorId("aa<div id='deg'>dsds<div>blin</div></div>bb", "deg"), QString("<div id='deg'>dsds<div>blin</div></div>"));
	QCOMPARE(selectorId("aa<div id='deg'>dsds<div id=\"hardcore\"><b>blin</b></div></div>bbb", "deg"), QString("<div id='deg'>dsds<div id=\"hardcore\"><b>blin</b></div></div>"));
	QCOMPARE(selectorId("aa<div id='deg'>dsds<div id=\"hardcore\"><b>blin</b></div></div>bbb", "hardcore"), QString("<div id=\"hardcore\"><b>blin</b></div>"));
	QCOMPARE(selectorId("<div id='ss'></div>aa<div id='deg'>dsds<div class='brolaf' id=\"hardcore\" name='weed'><b>blin</b></div></div>bbb", "hardcore"), QString("<div class='brolaf' id=\"hardcore\" name='weed'><b>blin</b></div>"));

	// wrong html
	QCOMPARE(selectorId("a<div id='ss'><div></div><div><div></div></div></div></div>c", "ss"), QString("<div id='ss'><div></div><div><div></div></div></div>"));
	QCOMPARE(selectorId("aa<div id='deg'>dsds<div></div>bb", "deg"), QString("<div id='deg'>dsds<div></div>"));
	
	// Bug
	QCOMPARE(selectorId("aa<div id='deg'><div id='lol'> <!-- <div id='welcome'></div> --> </div><div id='lol2'></div></div>bb", "deg"), QString("<div id='deg'><div id='lol'> <!-- <div id='welcome'></div> --> </div><div id='lol2'></div></div>"));
}



/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Globals)
#include "test-Globals.moc"
