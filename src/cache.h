/****************************
** Tsunagari Tile Engine   **
** cache.h                 **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <vector>

#include "log.h"

#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>

#define IN_USE_NOW -1

template<class T>
class Cache
{
public:
	T momentaryRequest(const std::string& name)
	{
		if (conf.cacheEnabled) {
			CacheMap::iterator it = map.find(name);
			if (it != map.end()) {
				Log::dbg("Cache", name + ": requested (cached)");
				CacheEntry& entry = it->second;
				// Set lastUsed to now because it won't be used
				// by the time garbageCollect() gets to it.
				int now = GameWindow::getWindow().time();
				entry.lastUsed = now;
				return entry.resource;
			}
		}
		Log::dbg("Cache", name + ": requested");
		return T();
	}

	T lifetimeRequest(const std::string& name)
	{
		if (conf.cacheEnabled) {
			CacheMap::iterator it = map.find(name);
			if (it != map.end()) {
				Log::dbg("Cache", name + ": requested (cached)");
				CacheEntry& entry = it->second;
				entry.lastUsed = IN_USE_NOW;
				return entry.resource;
			}
		}
		Log::dbg("Cache", name + ": requested");
		return T();
	}

	void put(const std::string& name, T data)
	{
		if (!conf.cacheEnabled)
			return;
		CacheEntry<T> entry;
		entry.resource = data;
		entry.lastUsed = 0;
		map[name] = entry;
	}

	void garbageCollect()
	{
		if (!conf.cacheEnabled)
			return;
		int now = GameWindow::getWindow().time();
		std::vector<std::string> dead;
		BOOST_FOREACH(typename CacheMap::value_type& i, map) {
			const std::string& name = i.first;
			CacheEntry& cache = i.second;
			bool unused = !cache.resource || cache.resource.unique();
			if (unused) {
				if (cache.lastUsed == IN_USE_NOW) {
					cache.lastUsed = now;
					//Log::dbg("Resourcer", name + ": unused");
				}
				else if (now < cache.lastUsed) {
					// Handle time overflow.
					cache.lastUsed = now;
				}
				else if (now > cache.lastUsed + conf.cacheTTL*1000) {
					dead.push_back(name);
					Log::dbg("Cache", name + ": purged");
				}
			}
		}
		BOOST_FOREACH(std::string& name, dead)
			map.erase(name);
	}

private:
	struct CacheEntry
	{
		T resource;
		int lastUsed;
		int memoryUsed;
	};

	typedef boost::unordered_map<const std::string, CacheEntry<T> > CacheMap;
	CacheMap map;
};

#endif

