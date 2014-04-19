#ifndef BYTECACHE_HPP
#define	BYTECACHE_HPP

#include "lrucache.hpp"
#include <QString>
#include "webdatacontainer.h"

namespace novastory {

class ByteCache : public cache::LRUCache<std::string, WebDataContainer> 
{
public:
	ByteCache(size_t bytes = 0) : LRUCache(0), maxBytes(bytes), m_currentSize(0)
	{
		
	};

	void put(const std::string& key, const WebDataContainer& value)
	{
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

	void onDeleteValue(const WebDataContainer& value_old) override
	{
		m_currentSize -= value_old.size();
	}

	void onChangeValue(const WebDataContainer& value_old, const WebDataContainer& value_new) override
	{
		m_currentSize -= value_old.size();
		m_currentSize += value_new.size();
	};
	void onInsertValue(const WebDataContainer& value_new) override
	{
		m_currentSize += value_new.size();
	};

private:
	size_t maxBytes;
	size_t m_currentSize;
};

} // namespace novastory

#endif	/* BYTECACHE_HPP */

