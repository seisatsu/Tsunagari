/****************************
** Tsunagari Tile Engine   **
** cache.h                 **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>

template<class Res>
struct CacheEntry
{
	Res resource;
	int lastUsed;
	int memoryUsed;
};

template<class Res>
typedef boost::unordered_map<const std::string, CacheEntry<Res> > CacheMap;

//! Garbage collect a map.
template<class Map, class MapValue>
void reclaim(Map& map)
{
	int now = GameWindow::getWindow().time();
	std::vector<std::string> dead;
	BOOST_FOREACH(typename Map::value_type& i, map) {
		const std::string& name = i.first;
		CacheEntry<MapValue>& cache = i.second;
		bool unused = !cache.resource || cache.resource.unique();
		if (unused) {
			if (!cache.lastUsed) {
				cache.lastUsed = now;
				//Log::dbg("Resourcer", name + ": unused");
			}
			else if (now < cache.lastUsed) {
				// Handle time overflow
				cache.lastUsed = now;
			}
			else if (now > cache.lastUsed + conf.cacheTTL*1000) {
				dead.push_back(name);
				Log::dbg("Resourcer", name + ": purged from cache");
			}
		}
		// XXX: Redundant? We're working around this because it won't
		// catch XML documents.
		else if (cache.lastUsed) {
			cache.lastUsed = 0;
			Log::dbg("Resourcer", name + ": requested (cached)");
		}
	}
	BOOST_FOREACH(std::string& name, dead)
		map.erase(name);
}

#endif

