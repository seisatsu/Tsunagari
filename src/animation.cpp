/*********************************
** Tsunagari Tile Engine        **
** animation.cpp                **
** Copyright 2011-2012 OmegaSDG **
*********************************/

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

