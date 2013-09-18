/***************************************
** Tsunagari Tile Engine              **
** gosu-image.h                       **
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

#ifndef GOSU_IMAGE_H
#define GOSU_IMAGE_H

#include "../image.h"

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

