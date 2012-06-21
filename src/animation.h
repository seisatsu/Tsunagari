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

/**
 * An Animation is a sequence of bitmap images (called frames) used to creates
 * the illusion of motion. Frames are cycled over with an even amount of time
 * given to each, and the whole animation starts over after the last frame is
 * displayed.
 *
 * Mechanically, it is a list of images and a period of time over which to
 * play.
 */
class Animation
{
public:
	/**
	 * Constructs an empty, but safe, Animation. All methods on this
	 * object will be null.
	 */
	Animation();

	/**
	 * Constructs a single-frame Animation. It will function like a static
	 * image.
	 *
	 * @param frame static image
	 */
	Animation(const ImageRef& frame);

	/**
	 * Constructs a Animation from a list of frames.
	 *
	 * If given more than one frame, frameLength must be a positive,
	 * non-zero value.
	 *
	 * @param frames list of frames to cycle through
	 * @param frameLength length of time in milliseconds that each frame
	 *        will display for
	 */
	Animation(const std::vector<ImageRef>& frames, int frameLength);

	/**
	 * Starts the animation over.
	 *
	 * @now current time in milliseconds
	 */
	void startOver(int now);

	/**
	 * Has this Animation switched frames since frame() was last called?
	 *
	 * @now current time in milliseconds
	 */
	bool needsRedraw(int now) const;

	/**
	 * Returns the image that should be displayed at this time.
	 *
	 * @now current time in milliseconds
	 */
	Gosu::Image* frame(int now);

private:

	typedef std::vector<ImageRef> ImageVec;


	/** List of images in animation. */
	ImageVec frames;

	/** Are we animated? Equals frames.size() > 1 */
	bool animated;

	/** Length of each frame in animation in milliseconds. */
	int frameLen;

	/** Length of one complete cycle through animation in milliseconds. */
	int animCycle;

	/** Index of frame currently displaying on screen. */
	int frameShowing;

	/** Time offset to find current animation frame. */
	int offset;
};

#endif

