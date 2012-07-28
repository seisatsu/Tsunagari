/****************************
** Tsunagari Tile Engine   **
** tiledimage.h            **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef TILEDIMAGE_H
#define TILEDIMAGE_H

#include <boost/shared_ptr.hpp>

#include "image.h"

class TiledImage
{
public:
	static TiledImage* create(void* data, size_t length,
			unsigned tileW, unsigned tileH);
	virtual ~TiledImage();

	virtual size_t size() const = 0;

	virtual ImageRef& operator[](size_t n) = 0;
	virtual const ImageRef& operator[](size_t n) const = 0;

private:
	TiledImage();

	friend class TiledImageImpl;
};

typedef boost::shared_ptr<TiledImage> TiledImageRef;

#endif

