/******************************
** Tsunagari Tile Engine     **
** door.h - Door             **
** Copyright 2011 OmegaSDG   **
******************************/

#include "tile.h"

class Door : public Tile
{
public:
	Door(Resource* rc, Gosu::Image* img, int size, bool walkable, coord_t coord, list<trigger>* triggers, Area* darea, coord_t dcoord);
	void activate(Entity* entity);

private:
	Area* darea;
	coord_t dcoord;
};

