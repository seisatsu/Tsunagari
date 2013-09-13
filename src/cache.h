/***************************************
** Tsunagari Tile Engine              **
** cache.h                            **
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

#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <string>

template<class T>
class Cache
{
public:
	T momentaryRequest(const std::string& name);

	T lifetimeRequest(const std::string& name);

	void momentaryPut(const std::string& name, T data);

	void lifetimePut(const std::string& name, T data);

	void garbageCollect();

private:
	struct CacheEntry
	{
		T resource;
		time_t lastUsed;
		size_t memoryUsed;
	};

	typedef std::map<const std::string, CacheEntry> CacheMap;
	typedef typename CacheMap::iterator CacheMapIter;
	CacheMap map;
};

#endif
