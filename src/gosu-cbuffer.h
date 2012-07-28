/****************************
** Tsunagari Tile Engine   **
** gosu-cbuffer.h          **
** Copyright 2012 OmegaSDG **
****************************/

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

