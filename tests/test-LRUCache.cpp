#include <QtTest>
#include "utils/lrucache.hpp"

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

	for (int i = 0; i < NUM_OF_TEST1_RECORDS; ++i) {
		cache.put(i, i);
	}

	for (int i = 0; i < NUM_OF_TEST1_RECORDS; ++i) {
		//if (!cache.exists(i)) assert(false);
		QVERIFY(cache.exists(i));
		QCOMPARE(cache.get(i), i);
	}

	QCOMPARE(cache.size(), (size_t)NUM_OF_TEST1_RECORDS);
}

void Test_LRUCache::numTest2()
{
	cache::LRUCache<int, int> cache(TEST2_CACHE_CAPACITY);

	for (int i = 0; i < NUM_OF_TEST2_RECORDS; ++i) {
		cache.put(i, i);
	}

	for (int i = 0; i < NUM_OF_TEST2_RECORDS - TEST2_CACHE_CAPACITY; ++i) {
		QVERIFY(!cache.exists(i));
	}

	for (int i = NUM_OF_TEST2_RECORDS - TEST2_CACHE_CAPACITY; i < NUM_OF_TEST2_RECORDS; ++i) {
		QVERIFY(cache.exists(i));
		QCOMPARE(cache.get(i), i);
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

	cache.put("four", "four");
	QVERIFY(cache.exists("two"));
	QVERIFY(cache.exists("three"));
	QVERIFY(cache.exists("four"));

	QCOMPARE(cache.get("four"), std::string("four"));

	bool error_caught = false;
	try{
		cache.get("one");
	} catch (std::range_error&) {
		error_caught = true;
	}
	QVERIFY(error_caught);
}


/********************** DECLARE_TEST LIST ****************************/
QTEST_MAIN(Test_LRUCache)
#include "test-LRUCache.moc"
