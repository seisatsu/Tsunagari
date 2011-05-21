/******************************
** Tsunagari Tile Engine     **
** tile.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include "tile.h"

Tile::Tile(Sprite* s, bool walkable, coord_t coords)
{
	this->s = s;
	this->walkable = walkable;
	this->coords = coords;
}

bool Tile::canWalk()
{
	return walkable;
}

void Tile::draw()
{
	s->draw();
}

