/****************************
** Tsunagari Tile Engine   **
** image-impl.h            **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef IMAGE_IMPL_H
#define IMAGE_IMPL_H

#include "image.h"

namespace Gosu { class Bitmap; }
namespace Gosu { class Image; }

class ImageImpl : public Image
{
public:
	ImageImpl();
	~ImageImpl();

	bool init(void* data, size_t length);
	bool init(Gosu::Bitmap& bitmap, unsigned x, unsigned y,
	                                unsigned w, unsigned h);

	void draw(double dstX, double dstY, double z) const;
	void drawSubrect(double dstX, double dstY, double z,
	                 double srcX, double srcY,
	                 double srcW, double srcH);

	unsigned width() const;
	unsigned height() const;

private:
	Gosu::Image* img;
};

#endif

