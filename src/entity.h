/****************************
** Tsunagari Tile Engine   **
** entity.h - Entity       **
** Copyright 2011 OmegaSDG **
****************************/

#include "area.h"
#include "entity.h"
#include "resourcer.h"
#include "tilegrid.h"
#include "world.h"

class Entity
{
public:
	Entity(World* world, Resource* rc);
	void set_area(Area* area);
	void go_to(Area* area, coord_t coords);
	coord_t get_coords();
	void transport(coord_t coords);
	void move(int dx, int dy);
	void draw();
	bool needs_redraw();

protected:
	World* world;
	TileGrid* grid;
	Gosu::Image img;
	bool redraw;
};

