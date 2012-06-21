/*********************************
** Tsunagari Tile Engine        **
** animation.cpp                **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <Gosu/Graphics.hpp>

#include "animation.h"

Animation::Animation()
	: animated(false),
	  frameShowing(0)
{
}

Animation::Animation(const ImageRef& frame)
	: animated(false),
	  frameShowing(0)
{
	frames.push_back(frame);
}

Animation::Animation(const std::vector<ImageRef>& _frames, int frameLen)
	: frameLen(frameLen),
	  frameShowing(0),
	  offset(0)
{
	for (ImageVec::const_iterator it = _frames.begin(); *it; it++)
		frames.push_back(*it);
	animated = frames.size() > 1;
	animCycle = frameLen * (int)frames.size();
}

void Animation::startOver(int now)
{
	if (animated) {
		offset = now;
		frameShowing = 0;
	}
}

bool Animation::needsRedraw(int now) const
{
	if (animated) {
		int pos = now - offset;
		int frame = (pos % animCycle) / frameLen;
		return frame != frameShowing;
	}
	return false;
}

Gosu::Image* Animation::frame(int now)
{
	switch (frames.size()) {
	case 0:
		return NULL;
	case 1:
		return frames[0].get();
	default:
		int pos = now - offset;
		frameShowing = (pos % animCycle) / frameLen;
		return frames[frameShowing].get();
	}
}

