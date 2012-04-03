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
	  animCycle(0),
	  frameShowing(0),
	  base(0)
{
}

void Animation::addFrame(ImageRef frame)
{
	frames.push_back(frame);
	animCycle = frameLen * (int)frames.size();
	if (frames.size() == 1)
		img = frame.get();
	if (frames.size() > 1)
		animated = true;
}

void Animation::setFrameLen(int milliseconds)
{
	frameLen = milliseconds;
	animCycle = frameLen * (int)frames.size();
}

void Animation::startOver(int ms_now)
{
	base = ms_now;
	frameShowing = 0;
	img = frames[0].get();
}

bool Animation::needsRedraw(int milliseconds) const
{
	if (animated) {
		int offset = milliseconds - base;
		int frame = (offset % animCycle) / frameLen;
		return frame != frameShowing;
	}
	return false;
}

void Animation::updateFrame(int milliseconds)
{
	if (animated) {
		int offset = milliseconds - base;
		frameShowing = (offset % animCycle) / frameLen;
		img = frames[frameShowing].get();
	}
}

Gosu::Image* Animation::frame() const
{
	return img;
}

