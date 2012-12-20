/****************************
** Tsunagari Tile Engine   **
** readercache.h           **
** Copyright 2012 OmegaSDG **
****************************/

// "OmegaSDG" is defined as Michael D. Reiley and Paul Merrill.

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

#ifndef READERCACHE_H
#define READERCACHE_H

#include <string>
#include <vector>

#include "cache.h"
#include "log.h"

template<class T>
class ReaderCache
{
public:
	typedef T (*GenFn)(const std::string& name);

	ReaderCache(GenFn fn) : fn(fn) {}

	T momentaryRequest(const std::string& name)
	{
		T t = cache.momentaryRequest(name);
		if (t)
			return t;

		t = fn(name);
		cache.momentaryPut(name, t);
		return t;
	}

	T lifetimeRequest(const std::string& name)
	{
		T t = cache.lifetimeRequest(name);
		if (t)
			return t;

		t = fn(name);
		cache.lifetimePut(name, t);
		return t;
	}

	void garbageCollect()
	{
		cache.garbageCollect();
	}

private:
	GenFn fn;

	Cache<T> cache;
};

#endif

