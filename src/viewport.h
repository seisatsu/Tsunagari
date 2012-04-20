/*********************************
** Tsunagari Tile Engine        **
** viewport.h                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "entity.h"

class Area;

//! General control over where and how the map is rendered.
/*!
	
 */
class Viewport
{
public:
	Viewport(icoord vsize);
	~Viewport();

	void update(unsigned long dt);

	//! How far the map is scrolled in pixels, counting from the upper-left.
	rvec2 getMapOffset() const;

	//! Size of the letterbox matte bars in pixels.
	rvec2 getLetterboxOffset() const;

	//! Multiplier in X and Y dimensions to get from virtRes to physRes.
	rvec2 getScale() const;

	//! The resolution our game is actually being drawn at.
	rvec2 getPhysRes() const;

	//! The resolution our game thinks it is being drawn at. Chosen by a
	//! world's creator. This allows graphics to look the same on any
	//! setups of any resolution.
	rvec2 getVirtRes() const;

	// Immediatly center render offset. Stop any tracking.
	void jumpToPt(ivec2 pt);
	void jumpToPt(rvec2 pt);
	void jumpToEntity(const Entity* e);

	// Continuously follow.
	void trackEntity(const Entity* e);

	void setArea(const Area* a);

private:
	void _jumpToEntity(const Entity* e);

	//! Returns as a normalized vector the percentage of screen that should
	//! be blanked to preserve the aspect ratio. It can also be thought of
	//! as the correcting aspect ratio.
	rvec2 getLetterbox() const;

	rvec2 offsetForPt(rvec2 pt) const;
	rvec2 centerOn(rvec2 pt) const;
	rvec2 boundToArea(rvec2 pt) const;
	double boundDimension(double window, double area, double pt,
	                      bool loop) const;
	rvec2 addLetterboxOffset(rvec2 pt) const;

	enum TrackingMode
	{
		TM_MANUAL,
		TM_FOLLOW_ENTITY
	};

	double aspectRatio;
	rvec2 off;
	rvec2 virtRes;

	TrackingMode mode;
	const Area* area;
	const Entity* targete;
};

#endif

