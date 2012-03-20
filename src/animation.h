/*********************************
** Tsunagari Tile Engine        **
** animation.h                  **
** Copyright 2011-2012 OmegaSDG **
*********************************/

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

	void addFrame(ImageRef frame);
	void setFrameLen(int milliseconds);
	void startOver(int ms_now);

	bool needsRedraw(int milliseconds) const;
	void updateFrame(int milliseconds);
	Gosu::Image* frame() const;

private:
	/** List of images in animation. */
	std::vector<ImageRef> frames;

	/** Current graphic displaying on screen. */
	Gosu::Image* img;

	/** Are we animated? Equals frames.size() > 1 */
	bool animated;

	/** Length of each frame in animation in milliseconds. */
	int frameLen;

	/** Length of one complete cycle through animation in milliseconds. */
	int animCycle;

	/** Index of frame currently displaying on screen. */
	int frameShowing;

	/** Millisecond base to find current animation frame. */
	int base;
};

#endif

