#include "viewport.h"


Viewport::Viewport(const ClientValues* conf)
	: conf(conf), mode(TM_MANUAL)
{
}

Viewport::~Viewport()
{
}


void Viewport::update(unsigned long)
{
	rcoord r;

	switch (mode) {
	case TM_MANUAL:
		// Do nothing.
		break;
	case TM_FOLLOW_ENTITY:
		r = targete->getRPixel();
		off.x = r.x;
		off.y = r.y;
		break;
	case TM_SCROLL_TO_ENTITY:
		// TODO
		break;
	case TM_SCROLL_TO_TILE:
		// TODO
		break;
	}
}

rvec2 Viewport::getRenderOffset() const
{
	return off;
}


// Immediatly center render offset. Stop any scrolling or tracking.
void Viewport::jumpToXY(ivec2 off)
{
	mode = TM_MANUAL;
	this->off.x = off.x;
	this->off.y = off.y;
}

void Viewport::jumpToXY(rvec2 off)
{
	mode = TM_MANUAL;
	this->off = off;
}

void Viewport::jumpToEntity(const Entity& e)
{
	mode = TM_MANUAL;
	const rcoord r = e.getRPixel();
	off.x = r.x;
	off.y = r.y;
}


// Move over a duration. Stop any tracking.
void Viewport::scrollToEntity(const Entity& e)
{
	mode = TM_SCROLL_TO_ENTITY;
	targete = &e;
}

void Viewport::scrollToTile(icoord c)
{
	mode = TM_SCROLL_TO_TILE;
	targetc = c;
}


// Continuously follow. Stop any scrolling.
void Viewport::trackEntity(const Entity& e)
{
	mode = TM_FOLLOW_ENTITY;
	targete = &e;
}

