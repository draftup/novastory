#include <QtTest>
#include "webserver/webdatacontainer.h"

using namespace novastory;

class Test_WebDataContainer: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void mainTest();
	void etagTest();
	void detectDataTest();
private:

	QString data;
};

void Test_WebDataContainer::initTestCase()
{
	data =
		"data:image/gif;base64,R0lGODlhEAAQAPecAMoXF8sXF9kZGf39/d8aGrgVFexbWtgZGeUcHNQYGNwZG"
		"cQWFv38/McXF8kXF/Xy8uDb28AWFvWpp+UeHe94eeU6O8gfHu1nZPbe3skdHeQaHNFdXPXW1PWqq7EWFtAY"
		"GO5ub97IyO55evzp6ck2NuhOUO5sbK4UFPDm5vn29udUVutaWNi5ub84OfCBf8iFhfn398NRUt4ZGexZWvz8"
		"/OfU1L5JSePQ0P79/cJCQtx/gKWMi91AP+tiZfGMju5zb8sbHfv6+vv5+e1oZ9NFQOlCQecvLbpXV7t2dfB9g"
		"s0XF7J7evGHhMUXF/ayt+9zdMIWFtkdH9cZGedNUMMWFtUYGOxdXModHc2SktEZGOU3OtNVVexlZcCbnPrl5fz"
		"6+ulTUs00MvGgoslBQM5CQ74XGM2Jh+xgZeEaGvnPzc+am+vn5+xpaMN+fd9OS7cVFfrW09qnorFxccGbm+Z"
		"YV/GJi+5vcL4bH95fXupTU9YYGN0ZGf37+8I9O+1iY+peX/GLi+tbVt8sLNMYGNsbHPOZm9WrrM+oqNMqKex"
		"dYNoaGepaXOxeXeJQU/vq6/OxrtDBwdJ3ePLMys8YGc4YGPj39+h6erwVFdgeH599fswXGNIYGP///wAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAJwALAAAAAAQABAAAAjgADkJ5"
		"MQHx8CDCFGo4YDBUBAGXeJIMjNg4I1HHQAVSuPF0QgnEuCIqcQJhiUKfuw8GWLiQqBEjIr4UDGABRsDBs78c"
		"MGkThIQVsCsEAGhzQw6f0oYmaABAYIKXHo0WpQJyRQehBTsQUOAgAwFiqK4ybPjkKBNCfQcEMD2gJQESrJoW"
		"fMgwyAAHzbp1VuFUgMCV2hwwoIJgAMAARIbbqJpEiSBA2IgKgNlwYIGVCJYALLkoJAcRO4UuFTgDYkwchBy+"
		"nIET58THjaQmaN64IstkXSMCVH7YI0WNlL0Rsigd0AAOw==";
}

void Test_WebDataContainer::init()
{

}

void Test_WebDataContainer::cleanup()
{

}

void Test_WebDataContainer::cleanupTestCase()
{

}

void Test_WebDataContainer::mainTest()
{
	WebDataContainer img(data);

	QFile f(QCoreApplication::applicationDirPath() + "/rating_1_on.gif");
	QVERIFY(f.open(QIODevice::ReadOnly));
	qDebug() << img.size();
	QVERIFY(img == f.readAll());

	QCOMPARE(img.toString(), data);
}

void Test_WebDataContainer::etagTest()
{
	WebDataContainer img(data);
	img.setModificatedDate(QDateTime::fromMSecsSinceEpoch(5));
	QString eTag = img.eTag();
	QVERIFY(eTag.size() > 0);
	img.setModificatedDate(QDateTime::fromMSecsSinceEpoch(10));
	QVERIFY(eTag != img.eTag());
	img.setModificatedDate(QDateTime::fromMSecsSinceEpoch(5));
	QCOMPARE(img.eTag(), eTag);
	img = QByteArray("dasdasdasd");
	img.setModificatedDate(QDateTime::fromMSecsSinceEpoch(5));
	QVERIFY(eTag != img.eTag());
	img.setRFCData(data);
	QCOMPARE(img.eTag(), eTag);
}

void Test_WebDataContainer::detectDataTest()
{
	WebDataContainer img(data);
	img.setMimeType("notimage");
	QCOMPARE(img.mimeType(), QString("notimage"));
	QVERIFY(img.detectMimeType());
	QCOMPARE(img.mimeType(), QString("image/gif"));
}


/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_WebDataContainer)
#include "test-WebDataContainer.moc"
