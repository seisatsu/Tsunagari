/****************************
** Tsunagari Tile Engine   **
** gosu-cbuffer.cpp        **
** Copyright 2012 OmegaSDG **
****************************/

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
