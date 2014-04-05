#include <QtTest>
#include "recaptcha.h"
#include "logic/captcha.h"
#include "utils/globals.h"
#include "sql/sqldatabase.h"

using namespace novastory;

class Test_Recaptcha: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void stubTest();
	void notifyTest();
	void insertTest();
	void generateTokenTest();
private:
};

void Test_Recaptcha::initTestCase()
{
	SqlDatabase::Instance();
	SqlQuery q;
	QVERIFY(q.exec("DELETE FROM usersverify WHERE email = 'dsdasd@gmail.com'"));
}

void Test_Recaptcha::init()
{

}

void Test_Recaptcha::cleanup()
{

}

void Test_Recaptcha::cleanupTestCase()
{

}

void Test_Recaptcha::stubTest()
{
	Recaptcha* cha = new Recaptcha("dasdasdasd", "dasdasd", QHostAddress("10.10.10.5"));
	QVERIFY(!cha->checkCaptchaSync());
	cha->deleteLater();
}

void Test_Recaptcha::notifyTest()
{
	Captcha cha;
	cha.setEmail("dsdasd@gmail.com");
	cha.setPassword(sha1("lol"));
	cha.setResponse("dsadasd");
	cha.setChallenge("dasdasd");
	cha.setRemoteIP(QHostAddress("127.0.0.1"));
	QVERIFY(!cha.addVerifyNotify());
	QCOMPARE(cha.jsonErrorType(), 5);
}

void Test_Recaptcha::insertTest()
{
	Captcha cha;
	cha.setEmail("dsdasd@gmail.com");
	cha.setPassword(sha1("lol"));
	cha.setResponse("dsadasd");
	cha.setChallenge("dasdasd");
	cha.setRemoteIP(QHostAddress("127.0.0.1"));
	QVERIFY(!cha.addVerifyNotify()); // captcha verification failded, but salt and token must be ok
	QVERIFY(cha.insertSQL());
}

void Test_Recaptcha::generateTokenTest()
{
	Captcha cha;
	QVERIFY(!cha.generateToken().isEmpty());
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Recaptcha)
#include "test-Recaptcha.moc"
