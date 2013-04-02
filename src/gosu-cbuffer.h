/*********************************
** Tsunagari Tile Engine        **
** gosu-cbuffer.h               **
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

#ifndef CBUFFER_H
#define CBUFFER_H

#include <Gosu/IO.hpp>

namespace Gosu {

/**
 * Similar to Gosu::Buffer, but can be constructed around pre-existing C void*.
 * The memory is not copied. Also, this is a read-only implementation. If you
 * attempt to create a Gosu::Writer around this, it will fail.
 *
 * See Gosu/IO.hpp and GosuImpl/IO.cpp from Gosu
 * See http://www.libgosu.org/cpp/class_gosu_1_1_buffer.html
 * See http://www.libgosu.org/cpp/class_gosu_1_1_resource.html
 */
class CBuffer : public Resource
{
public:
	CBuffer(void *data, size_t len);

	std::size_t size() const;
	void resize(std::size_t); // not implemented

	void read(std::size_t offset, std::size_t length,
			void* destBuffer) const;
	void write(std::size_t, std::size_t, const void *); // not implemented

private:
	// Non-copyable.
	CBuffer(const CBuffer& other);
	CBuffer& operator=(const CBuffer& other);

private:
	const void* data;
	size_t len;
};

} // namespace Gosu

#endif

