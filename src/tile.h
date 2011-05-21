/******************************
** Tsunagari Tile Engine     **
** tile.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef TILE_H
#define TILE_H

#include <list>

#include "entity.h"
#include "sprite.h"

class Entity;
class Sprite;

class Tile
{
public:
	Tile(Sprite* s, bool walkable, coord_t coords);
	bool canWalk();
	void draw();
	
private:
	Sprite* s;
	bool walkable;
	coord_t coords;

	std::list<Entity*> entities;
};

#endif

