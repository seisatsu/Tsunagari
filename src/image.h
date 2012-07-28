/****************************
** Tsunagari Tile Engine   **
** image.h                 **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef IMAGE_H
#define IMAGE_H

#include <cstring> // for size_t

#include <boost/shared_ptr.hpp>

class Image
{
public:
	static Image* create(void* data, size_t length);
	virtual ~Image() = 0;

	virtual void draw(double dstX, double dstY, double z) const = 0;
	virtual void drawSubrect(double dstX, double dstY, double z,
	                 double srcX, double srcY,
	                 double srcW, double srcH) = 0;

	virtual unsigned width() const = 0;
	virtual unsigned height() const = 0;

private:
	Image();

	friend class ImageImpl;
};

typedef boost::shared_ptr<Image> ImageRef;

#endif

