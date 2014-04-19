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
			_cache_items_list.erase(it->second);
			_cache_items_map.erase(it);
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
	
	size_t size() const {
		return _cache_items_map.size();
	}
protected:
	virtual bool needCleanup() const
	{
		return _max_size > 0 && _cache_items_map.size() > 0 && _cache_items_map.size() > _max_size;
	}

	virtual value_t cleanup()
	{
		auto last = _cache_items_list.end();
		last--;
		value_t deleted_value = last->second;
		_cache_items_map.erase(last->first);
		_cache_items_list.pop_back();
		return deleted_value;
	}

	std::mutex _mutex;
private:
	std::list<key_value_pair_t> _cache_items_list;
	std::unordered_map<key_t, list_iterator_t> _cache_items_map;
	size_t _max_size;
};

} // namespace lru

#endif	/* _LRUCACHE_HPP_INCLUDED_ */

