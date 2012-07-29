/****************************
** Tsunagari Tile Engine   **
** tiledimage.h            **
** Copyright 2012 OmegaSDG **
****************************/

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

