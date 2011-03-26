/****************************
** Tsunagari Tile Engine   **
** entity.h - Entity       **
** Copyright 2011 OmegaSDG **
****************************/

#ifndef ENTITY_H
#define ENTITY_H

#include <string>

//#include "area.h"
#include "resourcer.h"
//#include "tilegrid.h"
//#include "world.h"

class Resourcer;

using std::wstring;

class Entity
{
public:
	Entity(Resourcer* rc, wstring img_fn);
	void draw();
/*
	Entity(World* world, Resource* rc);
	void set_area(Area* area);
	void go_to(Area* area, coord_t coords);
	coord_t get_coords();
	void transport(coord_t coords);
	void move(int dx, int dy);
	bool needs_redraw();
*/

protected:
	Gosu::Image* img;
/*
	World* world;
	TileGrid* grid;
	bool redraw;
*/
};

#endif

