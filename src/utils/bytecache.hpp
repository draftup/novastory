#ifndef BYTECACHE_HPP
#define	BYTECACHE_HPP

#include "lrucache.hpp"
#include <QString>
#include <QByteArray>

namespace novastory {

class ByteCache : public cache::LRUCache<std::string, QByteArray> 
{
public:
	ByteCache(size_t bytes = 0) : LRUCache(0), maxBytes(bytes), m_currentSize(0)
	{
		
	};

	void put(const std::string& key, const QByteArray& value)
	{
		m_currentSize += value.size();
		LRUCache::put(key, value);
	}

	size_t maxSize() const
	{
		return maxBytes;
	};

	void setMaxSize(size_t size)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		maxBytes = size;
		while(needCleanup())
			cleanup();
	}

	size_t currentSize() const
	{
		return m_currentSize;
	}
protected:
	bool needCleanup() const override
	{
		return LRUCache::needCleanup() || (maxBytes > 0 && m_currentSize > maxBytes);
	}
	QByteArray cleanup() override
	{
		QByteArray ret = LRUCache::cleanup();
		m_currentSize -= ret.size();
		return ret;
	}
private:
	size_t maxBytes;
	size_t m_currentSize;
};

} // namespace novastory

#endif	/* BYTECACHE_HPP */

