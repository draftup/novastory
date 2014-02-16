#include <QtTest>
#include "recaptcha.h"

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

/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_Recaptcha)
#include "test-Recaptcha.moc"
