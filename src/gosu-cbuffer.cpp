/*********************************
** Tsunagari Tile Engine        **
** gosu-cbuffer.cpp             **
** Copyright 2011-2012 OmegaSDG **
*********************************/

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

#include <algorithm>
#include <cassert>
#include <string.h>

#include "gosu-cbuffer.h"

Gosu::CBuffer::CBuffer(void *data, size_t len)
	: data(data), len(len)
{
}

std::size_t Gosu::CBuffer::size() const
{
	return len;
}

void Gosu::CBuffer::resize(std::size_t)
{
	assert(0 && "not implemented");
}

void Gosu::CBuffer::read(std::size_t offset, std::size_t length,
		void* destBuffer) const
{
	size_t avail = std::min(this->len, length);
	memcpy(destBuffer, (char*)data + offset, avail);
}

void Gosu::CBuffer::write(std::size_t, std::size_t, const void*)
{
	assert(0 && "not implemented");
}
