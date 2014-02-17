#include <QtTest>
#include "recaptcha.h"
#include "logic/captcha.h"

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
private:
};

void Test_Recaptcha::initTestCase()
{

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
	QVERIFY(!cha.addVerifyNotify());
}

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Recaptcha)
#include "test-Recaptcha.moc"
