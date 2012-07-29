/*********************************
** Tsunagari Tile Engine        **
** animation.cpp                **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <cassert>

#include <Gosu/Graphics.hpp>

#include "animation.h"

Animation::Animation()
	: cycles(0),
	  frameShowing(0)
{
}

Animation::Animation(const ImageRef& frame)
	: cycles(0),
	  frameShowing(0)
{
	frames.push_back(frame);
}

Animation::Animation(const std::vector<ImageRef>& _frames, time_t frameTime)
	: cycles(0),
	  frameTime(frameTime),
	  frameShowing(0),
	  offset(0)
{
	ImageVec::const_iterator it;
	for (it = _frames.begin(); it != _frames.end(); it++)
		frames.push_back(*it);
	if (cycles == 0 && frames.size() > 1)
		cycles = ANIM_INFINITE_CYCLES;
	cycleTime = frameTime * frames.size();
}

void Animation::startOver(time_t now, int cycles)
{
	assert(cycles >= 0 || cycles == ANIM_INFINITE_CYCLES);

	this->cycles = cycles;
	offset = now;
	frameShowing = 0;
}

bool Animation::needsRedraw(time_t now) const
{
	if (cycles) {
		time_t pos = now - offset;
		size_t frame = (pos % cycleTime) / frameTime;
		return frame != frameShowing;
	}
	return false;
}

Image* Animation::frame(time_t now)
{
	if (frames.size() == 0)
		return NULL;
	if (cycles == 0)
		return frames[frameShowing].get();

	time_t pos = now - offset;
	frameShowing = (pos % cycleTime) / frameTime;

	if (cycles != ANIM_INFINITE_CYCLES && pos / cycleTime >= cycles) {
		cycles = 0;
		frameShowing = frames.size() - 1; // last frame
	}

	return frames[frameShowing].get();
}

