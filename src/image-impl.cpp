/*********************************
** Tsunagari Tile Engine        **
** image-impl.cpp               **
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

#include <cassert>

#include <Gosu/Bitmap.hpp>
#include <Gosu/Graphics.hpp>
#include <Gosu/Image.hpp>

#include "gosu-cbuffer.h"
#include "image-impl.h"
#include "window.h"


Image* Image::create(void* data, size_t length)
{
	ImageImpl* ii = new ImageImpl;
	if (ii->init(data, length))
		return ii;
	else {
		delete ii;
		return NULL;
	}
}


ImageImpl::ImageImpl()
	: img(NULL)
{
}

ImageImpl::~ImageImpl()
{
	delete img;
}

bool ImageImpl::init(void* data, size_t length)
{
	assert(img == NULL);

	Gosu::Graphics& graphics = GameWindow::instance().graphics();
	Gosu::CBuffer buffer(data, length);
	Gosu::Bitmap bitmap;

	Gosu::loadImageFile(bitmap, buffer.frontReader());
	img = new Gosu::Image(graphics, bitmap, false);

	return true;
}

bool ImageImpl::init(Gosu::Bitmap& bitmap, unsigned x, unsigned y,
				unsigned w, unsigned h)
{
	assert(img == NULL);

	Gosu::Graphics& graphics = GameWindow::instance().graphics();
	img = new Gosu::Image(graphics, bitmap, x, y, w, h, false);
	return true;
}


void ImageImpl::draw(double dstX, double dstY, double z) const
{
	assert(img != NULL);

	img->draw(dstX, dstY, z);
}

void ImageImpl::drawSubrect(double dstX, double dstY, double z,
		 double srcX, double srcY,
		 double srcW, double srcH)
{
	assert(img != NULL);

	Gosu::Graphics& g = GameWindow::instance().graphics();
	g.beginClipping(dstX + srcX, dstY + srcY, srcW, srcH);
	draw(dstX, dstY, z);
	g.endClipping();
}


unsigned ImageImpl::width() const
{
	assert(img != NULL);

	return img->width();
}

unsigned ImageImpl::height() const
{
	assert(img != NULL);

	return img->height();
}

