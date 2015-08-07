/* 
 * File:   lrucache.hpp
 * Author: Alexander Ponomarev
 * Modificated: Alexey Kasyanchuk
 * 
 * Created on June 20, 2013, 5:09 PM
 */

#ifndef LRUCACHE_HPP
#define	LRUCACHE_HPP

#include <unordered_map>
#include <list>
#include <cstddef>
#include <stdexcept>
#include <mutex>

namespace cache {

template<typename key_t, typename value_t>
class LRUCache {
public:
	typedef typename std::pair<key_t, value_t> key_value_pair_t;
	typedef typename std::list<key_value_pair_t>::iterator list_iterator_t;

	LRUCache(size_t max_size = 0) :
		_max_size(max_size) {
	}
	
	void put(const key_t& key, const value_t& value) {
		std::lock_guard<std::mutex> lock(_mutex);
		auto it = _cache_items_map.find(key);
		if (it != _cache_items_map.end()) {
			onChangeValue(it->second->second, value);
			_cache_items_list.erase(it->second);
			_cache_items_map.erase(it);
		}
		else
		{
			onInsertValue(value);
		}
			
		_cache_items_list.push_front(key_value_pair_t(key, value));
		_cache_items_map[key] = _cache_items_list.begin();
		
		while (needCleanup()) {
			cleanup();
		}
	}
	
	const value_t& get(const key_t& key) {
		std::lock_guard<std::mutex> lock(_mutex);
		auto it = _cache_items_map.find(key);
		if (it == _cache_items_map.end()) {
			throw std::range_error("There is no such key in cache");
		} else {
			_cache_items_list.splice(_cache_items_list.begin(), _cache_items_list, it->second);
			return it->second->second;
		}
	}
	
	bool exists(const key_t& key) const {
		return _cache_items_map.find(key) != _cache_items_map.end();
	}
	
	void remove(const key_t& key)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		auto it = _cache_items_map.find(key);
		if(it != _cache_items_map.end())
		{
			onDeleteValue(it->second->second);
			_cache_items_list.erase(it->second);
			_cache_items_map.erase(it);
		}
	}

	size_t size() const {
		return _cache_items_map.size();
	}

	std::list<key_t> keys() const {
		std::list<key_t> key_list;
		for (typename std::unordered_map<key_t, list_iterator_t>::const_iterator it = _cache_items_map.begin(); it != _cache_items_map.end(); it++)
		{
			key_list.push_back(it->first);
		}
		return key_list;
	}
protected:
	virtual bool needCleanup() const
	{
		return _max_size > 0 && _cache_items_map.size() > 0 && _cache_items_map.size() > _max_size;
	}

	inline void cleanup()
	{
		auto last = _cache_items_list.end();
		last--;
		onDeleteValue(last->second);
		_cache_items_map.erase(last->first);
		_cache_items_list.pop_back();
	}

	virtual void onDeleteValue(const value_t& value_old) {};
	virtual void onChangeValue(const value_t& value_old, const value_t& value_new) {};
	virtual void onInsertValue(const value_t& value_new) {};

	std::mutex _mutex;
private:
	std::list<key_value_pair_t> _cache_items_list;
	std::unordered_map<key_t, list_iterator_t> _cache_items_map;
	size_t _max_size;
};

} // namespace lru

#endif	/* _LRUCACHE_HPP_INCLUDED_ */

