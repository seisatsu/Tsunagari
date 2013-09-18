/***************************************
** Tsunagari Tile Engine              **
** gosu-tiledimage.cpp                **
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

#include <Gosu/Bitmap.hpp>

#include "gosu-cbuffer.h"
#include "gosu-image.h"
#include "gosu-tiledimage.h"
#include "../window.h"

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

