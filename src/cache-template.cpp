/***************************************
** Tsunagari Tile Engine              **
** cache.cpp                          **
** Copyright 2011-2013 PariahSoft LLC **
***************************************/

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

#include <vector>

#include "cache.h"
#include "client-conf.h"
#include "log.h"
#include "window.h"

#define IN_USE_NOW -1

template<class T>
T Cache<T>::momentaryRequest(const std::string& name)
{
	if (conf.cacheEnabled) {
		CacheMapIter it = map.find(name);
		if (it != map.end()) {
//			Log::info("Cache", name + ": requested (cached)");
			CacheEntry& entry = it->second;
			// Set lastUsed to now because it won't be used
			// by the time garbageCollect() gets to it.
			entry.lastUsed = GameWindow::instance().time();
			return entry.resource;
		}
	}
	Log::info("Cache", name + ": requested");
	return T();
}

template<class T>
T Cache<T>::lifetimeRequest(const std::string& name)
{
	if (conf.cacheEnabled) {
		typename CacheMap::iterator it = map.find(name);
		if (it != map.end()) {
			//				Log::info("Cache", name + ": requested (cached)");
			CacheEntry& entry = it->second;
			entry.lastUsed = IN_USE_NOW;
			return entry.resource;
		}
	}
	Log::info("Cache", name + ": requested");
	return T();
}

template<class T>
void Cache<T>::momentaryPut(const std::string& name, T data)
{
	if (!conf.cacheEnabled)
		return;
	CacheEntry entry;
	entry.resource = data;
	time_t now = GameWindow::instance().time();
	entry.lastUsed = now;
	map[name] = entry;
}
	
template<class T>
void Cache<T>::lifetimePut(const std::string& name, T data)
{
	if (!conf.cacheEnabled)
		return;
	CacheEntry entry;
	entry.resource = data;
	entry.lastUsed = IN_USE_NOW;
	map[name] = entry;
}
	
template<class T>
void Cache<T>::garbageCollect()
{
	if (!conf.cacheEnabled)
		return;
	time_t now = GameWindow::instance().time();
	typedef std::vector<std::string> StringVector;
	StringVector dead;
	for (CacheMapIter it = map.begin(); it != map.end(); it++) {
		const std::string& name = it->first;
		CacheEntry& cache = it->second;
		bool unused = !cache.resource || cache.resource.unique();
		if (!unused)
			continue;
		if (cache.lastUsed == IN_USE_NOW) {
			cache.lastUsed = now;
//			Log::info("Cache", name + ": unused");
		}
		else if (now > cache.lastUsed + conf.cacheTTL*1000) {
			dead.push_back(name);
			Log::info("Cache", name + ": purged");
		}
	}
	for (StringVector::iterator it = dead.begin(); it != dead.end(); it++)
		map.erase(*it);
}
