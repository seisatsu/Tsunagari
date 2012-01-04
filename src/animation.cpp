/*********************************
** Tsunagari Tile Engine        **
** animation.cpp                **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <Gosu/Graphics.hpp>

#include "animation.h"

Animation::Animation()
	: frames(),
	  img(NULL),
	  animated(false),
	  frameLen(0),
	  animLen(0),
	  frameShowing(0)
{
}

void Animation::addFrame(ImageRef frame)
{
	frames.push_back(frame);
	animLen = frameLen * (int)frames.size();
	if (frames.size() == 1)
		img = frame.get();
	if (frames.size() > 1)
		animated = true;
}

void Animation::setFrameLen(int milliseconds)
{
	frameLen = milliseconds;
	animLen = frameLen * (int)frames.size();
}

bool Animation::needsRedraw(int milliseconds) const
{
	if (animated) {
		int frame = (milliseconds % animLen) / frameLen;
		if (frame != frameShowing)
			return true;
	}
	return false;
}

void Animation::updateFrame(int milliseconds)
{
	if (animated) {
		frameShowing = (milliseconds % animLen) / frameLen;
		img = frames[frameShowing].get();
	}
}

Gosu::Image* Animation::frame() const
{
	return img;
}

