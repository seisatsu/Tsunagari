/******************************
** Tsunagari Tile Engine     **
** animation.h               **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef ANIMATED_H
#define ANIMATED_H

#include <vector>

#include "resourcer.h"

namespace Gosu {
	class Image;
}

class Animation
{
public:
	Animation();
	Animation(ImageRef frame);
	
	void addFrame(ImageRef frame);
	void setFrameLen(int milliseconds);

	bool needsRedraw(int milliseconds) const;
	void updateFrame(int milliseconds);
	Gosu::Image* frame() const;

private:
	//! List of images in animation.
	std::vector<ImageRef> frames;

	//! Current graphic displaying on screen.
	Gosu::Image* img;

	//! Are we animated? Same as frames.size() > 1
	bool animated;

	//! Length of each frame in animation.
	int frameLen;

	//! Total length of one complete cycle through animation.
	int animLen;

	//! Index of frame currently displaying on screen.
	int frameShowing;
};

#endif

