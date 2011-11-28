#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "common.h"
#include "entity.h"

//! General control over where and how the map is rendered.
/*!
	
 */
class Viewport
{
public:
	Viewport(const ClientValues* conf);
	~Viewport();

	void update(unsigned long dt);
	rvec2 getRenderOffset() const;

	// Immediatly center render offset. Stop any scrolling or tracking.
	void jumpToXY(ivec2 off);
	void jumpToXY(rvec2 off);
	void jumpToEntity(const Entity& e);

	// Move over a duration. Stop any tracking.
	void scrollToEntity(const Entity& e);
	void scrollToTile(icoord c);

	// Continuously follow. Stop any scrolling.
	void trackEntity(const Entity& e);

private:
	enum TrackingMode
	{
		TM_MANUAL,
		TM_FOLLOW_ENTITY,
		TM_SCROLL_TO_ENTITY,
		TM_SCROLL_TO_TILE
	};

	const ClientValues* conf;
	rvec2 off;

	TrackingMode mode;
	const Entity* targete;
	icoord targetc;
};

#endif

