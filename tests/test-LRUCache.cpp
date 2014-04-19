#include <QtTest>
#include "utils/lrucache.hpp"
#include "utils/bytecache.hpp"
#include <QByteArray>

using namespace cache;

class Test_LRUCache: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void init();
	void cleanup();
	void cleanupTestCase();

	void numTest1();
	void numTest2();
	void dataTest();
	void withoutLimit();
	void byteCache();
private:
};

void Test_LRUCache::initTestCase()
{

}

void Test_LRUCache::init()
{

}

void Test_LRUCache::cleanup()
{

}

void Test_LRUCache::cleanupTestCase()
{

}

const int NUM_OF_TEST1_RECORDS = 10000;
const int NUM_OF_TEST2_RECORDS = 10000;
const int TEST2_CACHE_CAPACITY = 2000;

void Test_LRUCache::numTest1()
{
	cache::LRUCache<int, int> cache(NUM_OF_TEST1_RECORDS);

	QBENCHMARK
	{
		for (int i = 0; i < NUM_OF_TEST1_RECORDS; ++i)
		{
			cache.put(i, i);
		}

		for (int i = 0; i < NUM_OF_TEST1_RECORDS; ++i)
		{
			//if (!cache.exists(i)) assert(false);
			QVERIFY(cache.exists(i));
			QCOMPARE(cache.get(i), i);
		}
	}

	QCOMPARE(cache.size(), (size_t)NUM_OF_TEST1_RECORDS);
}

void Test_LRUCache::numTest2()
{
	cache::LRUCache<int, int> cache(TEST2_CACHE_CAPACITY);

	QBENCHMARK
	{
		for (int i = 0; i < NUM_OF_TEST2_RECORDS; ++i)
		{
			cache.put(i, i);
		}

		for (int i = 0; i < NUM_OF_TEST2_RECORDS - TEST2_CACHE_CAPACITY; ++i)
		{
			QVERIFY(!cache.exists(i));
		}

		for (int i = NUM_OF_TEST2_RECORDS - TEST2_CACHE_CAPACITY; i < NUM_OF_TEST2_RECORDS; ++i)
		{
			QVERIFY(cache.exists(i));
			QCOMPARE(cache.get(i), i);
		}
	}

	QCOMPARE(cache.size(), (size_t)TEST2_CACHE_CAPACITY);
}

void Test_LRUCache::dataTest()
{
	cache::LRUCache<std::string, std::string> cache(3);
	cache.put("one", "one");
	cache.put("two", "two");
	cache.put("three", "three");

	QVERIFY(cache.exists("one"));
	QVERIFY(cache.exists("two"));
	QVERIFY(cache.exists("three"));

	QCOMPARE(cache.get("one"), std::string("one"));
	QCOMPARE(cache.get("two"), std::string("two"));
	QCOMPARE(cache.get("three"), std::string("three"));

	// Pop Test
	cache.put("four", "four");
	QVERIFY(cache.exists("two"));
	QVERIFY(cache.exists("three"));
	QVERIFY(cache.exists("four"));
	QCOMPARE(cache.get("four"), std::string("four"));
	bool error_caught = false;
	try
	{
		cache.get("one");
	}
	catch (std::range_error&)
	{
		error_caught = true;
	}

	// ordering test on get
	QCOMPARE(cache.get("two"), std::string("two"));
	cache.put("five", "five");
	QVERIFY(cache.exists("two"));

	QVERIFY(error_caught);
}

void Test_LRUCache::withoutLimit()
{
	cache::LRUCache<std::string, std::string> cache;
	cache.put("one", "onedata");
	cache.put("two", "twodata");
	QCOMPARE(cache.get("two"), std::string("twodata"));
}

void Test_LRUCache::byteCache()
{
	novastory::ByteCache cache(16);
	QByteArray v1; v1.fill('x', 8);
	QByteArray v2; v2.fill('y', 6);
	cache.put("1", v1);
	cache.put("2", v2);
	QVERIFY(cache.exists("1"));
	QVERIFY(cache.exists("2"));
	QByteArray v3; v3.fill('z', 1);
	cache.put("3", v3);
	QVERIFY(cache.exists("3"));
	v3.fill('p', 2);

	// Insert Test
	cache.put("4", v3);
	QVERIFY(cache.exists("4"));
	QVERIFY(!cache.exists("1"));
	QCOMPARE(cache.maxSize(), (size_t)16);
	QCOMPARE(cache.currentSize(), (size_t)9);
	QCOMPARE(static_cast<QByteArray>(cache.get("4")), QByteArray("pp"));

	// Change Test
	cache.put("2", v2);
	QCOMPARE(cache.currentSize(), (size_t)9); // must be same
	cache.put("2", v3);
	QCOMPARE(cache.currentSize(), (size_t)5);
	QCOMPARE(static_cast<QByteArray>(cache.get("3")), QByteArray("z"));
}


/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_LRUCache)
#include "test-LRUCache.moc"
