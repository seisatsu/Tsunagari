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

Animation::Animation(const std::vector<ImageRef>& _frames, time_t frameLen)
	: frameLen(frameLen),
	  frameShowing(0),
	  offset(0)
{
	ImageVec::const_iterator it;
	for (it = _frames.begin(); it != _frames.end(); it++)
		frames.push_back(*it);
	animated = frames.size() > 1;
	animCycle = frameLen * frames.size();
}

void Animation::startOver(time_t now)
{
	if (animated) {
		offset = now;
		frameShowing = 0;
	}
}

bool Animation::needsRedraw(time_t now) const
{
	if (animated) {
		time_t pos = now - offset;
		size_t frame = (pos % animCycle) / frameLen;
		return frame != frameShowing;
	}
	return false;
}

Image* Animation::frame(time_t now)
{
	switch (frames.size()) {
	case 0:
		return NULL;
	case 1:
		return frames[0].get();
	default:
		time_t pos = now - offset;
		frameShowing = (pos % animCycle) / frameLen;
		return frames[frameShowing].get();
	}
}

