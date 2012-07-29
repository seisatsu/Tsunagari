/****************************
** Tsunagari Tile Engine   **
** tiledimage-impl.cpp     **
** Copyright 2012 OmegaSDG **
****************************/

#include <Gosu/Bitmap.hpp>

#include "gosu-cbuffer.h"
#include "image-impl.h"
#include "tiledimage-impl.h"
#include "window.h"

TiledImage* TiledImage::create(void* data, size_t length,
		unsigned tileW, unsigned tileH)
{
	TiledImageImpl* tii = new TiledImageImpl;
	if (tii->init(data, length, tileW, tileH))
		return tii;
	else {
		delete tii;
		return NULL;
	}
}


bool TiledImageImpl::init(void* data, size_t length, unsigned tileW, unsigned tileH)
{
	Gosu::CBuffer buffer(data, length);
	Gosu::Bitmap bitmap;

	Gosu::loadImageFile(bitmap, buffer.frontReader());

	for (unsigned y = 0; y < bitmap.height(); y += tileH) {
		for (unsigned x = 0; x < bitmap.width(); x += tileW) {
			ImageImpl* img = new ImageImpl;
			if (img->init(bitmap, x, y, tileW, tileH))
				vec.push_back(ImageRef(img));
			else {
				delete img;
				return false;
			}
		}
	}

	return true;
}


size_t TiledImageImpl::size() const
{
	return vec.size();
}


ImageRef& TiledImageImpl::operator[](size_t n)
{
	return vec[n];
}

const ImageRef& TiledImageImpl::operator[](size_t n) const
{
	return vec[n];
}

