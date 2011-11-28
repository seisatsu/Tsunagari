/******************************
** Tsunagari Tile Engine     **
** viewport.h                **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "common.h"
#include "entity.h"

class Area;
class GameWindow;

//! General control over where and how the map is rendered.
/*!
	
 */
class Viewport
{
public:
	Viewport(const GameWindow& window, const ClientValues& conf);
	~Viewport();

	void update(unsigned long dt);
	rvec2 getOffset() const;

	// Immediatly center render offset. Stop any scrolling or tracking.
	void jumpToXY(ivec2 off);
	void jumpToXY(rvec2 off);
	void jumpToEntity(const Entity* e);

	// Move over a duration. Stop any tracking.
	void scrollToTile(icoord c);
	void scrollToEntity(const Entity* e);

	// Continuously follow. Stop any scrolling.
	void trackEntity(const Entity* e);

	void setArea(const Area* a);

private:
	rvec2 centerOn(rvec2 pt) const;

	enum TrackingMode
	{
		TM_MANUAL,
		TM_FOLLOW_ENTITY,
		TM_SCROLL_TO_ENTITY,
		TM_SCROLL_TO_TILE
	};

	const GameWindow& window;
	const ClientValues& conf;
	rvec2 off;

	TrackingMode mode;
	const Area* area;
	icoord targetc;
	const Entity* targete;
};

#endif

