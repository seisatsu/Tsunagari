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
	g.beginClipping(dstX + srcX, dstX + srcY, srcW, srcH);
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

