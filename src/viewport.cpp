/******************************
** Tsunagari Tile Engine     **
** viewport.cpp              **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Math.hpp>

#include "area.h"
#include "viewport.h"
#include "window.h"


Viewport::Viewport(const GameWindow& window, const ClientValues& conf)
	: window(window), conf(conf), mode(TM_MANUAL), area(NULL)
{
}

Viewport::~Viewport()
{
}


void Viewport::update(unsigned long)
{
	switch (mode) {
	case TM_MANUAL:
		// Do nothing.
		break;
	case TM_FOLLOW_ENTITY:
		_jumpToEntity(targete);
		break;
	case TM_SCROLL_TO_ENTITY:
		// TODO
		break;
	case TM_SCROLL_TO_TILE:
		// TODO
		break;
	}
}

rvec2 Viewport::getOffset() const
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

void Viewport::jumpToEntity(const Entity* e)
{
	mode = TM_MANUAL; // API implies mode change.
	_jumpToEntity(e);
}


// Move over a duration. Stop any tracking.
void Viewport::scrollToEntity(const Entity* e)
{
	mode = TM_SCROLL_TO_ENTITY;
	targete = e;
}

void Viewport::scrollToTile(icoord c)
{
	mode = TM_SCROLL_TO_TILE;
	targetc = c;
}


// Continuously follow. Stop any scrolling.
void Viewport::trackEntity(const Entity* e)
{
	mode = TM_FOLLOW_ENTITY;
	targete = e;
}


void Viewport::setArea(const Area* a)
{
	area = a;
}


void Viewport::_jumpToEntity(const Entity* e)
{
	rcoord pos = e->getPixelCoord();
	ivec2 td = area->getTileDimensions();
	rvec2 ecenter = rvec2(
		pos.x + td.x/2,
		pos.y + td.y/2
	);
	off = boundToArea(centerOn(ecenter));
}

rvec2 Viewport::centerOn(rvec2 pt) const
{
	const Gosu::Graphics& graphics = window.graphics();
	unsigned windowWidth = graphics.width();
	unsigned windowHeight = graphics.height();

	return rvec2(
		pt.x - windowWidth/2,
		pt.y - windowHeight/2
	);
}

rvec2 Viewport::boundToArea(rvec2 pt) const
{
	const Gosu::Graphics& graphics = window.graphics();
	double windowWidth = (double)graphics.width();
	double windowHeight = (double)graphics.height();
	icoord ad = area->getDimensions();
	ivec2 td = area->getTileDimensions();
	double areaWidth = ad.x * td.x;
	double areaHeight = ad.y * td.y;
	bool loopX = area->loopsInX();
	bool loopY = area->loopsInY();

	return rvec2(
		boundDimension(windowWidth,  areaWidth,  pt.x, loopX),
		boundDimension(windowHeight, areaHeight, pt.y, loopY)
	);
}

double Viewport::boundDimension(double window, double area, double pt,
                                bool loop) const
{
	// Since looping areas continue without bound, this is a no-op.
	if (loop)
		return pt;

	// If the Area is smaller than the screen, center the Area. Otherwise,
	// allow the screen to move to the edge of the Area, but not past.
	double wiggleRoom = area - window;
	return wiggleRoom <= 0 ?
	       wiggleRoom/2 :
	       Gosu::boundBy(pt, 0.0, wiggleRoom);
}

