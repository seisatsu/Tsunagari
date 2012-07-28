/****************************
** Tsunagari Tile Engine   **
** tiledimage-impl.h       **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef TILEDIMAGE_IMPL_H
#define TILEDIMAGE_IMPL_H

#include <vector>

#include "tiledimage.h"

class TiledImageImpl : public TiledImage
{
public:
	bool init(void* data, size_t length, unsigned tileW, unsigned tileH);

	size_t size() const;

	ImageRef& operator[](size_t n);
	const ImageRef& operator[](size_t n) const;

private:
	std::vector<ImageRef> vec;
};

#endif

