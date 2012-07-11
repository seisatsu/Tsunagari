/*********************************
** Tsunagari Tile Engine        **
** character.cpp                **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include "area.h"
#include "character.h"

Character::Character()
	: Entity()
{
}

Character::~Character()
{
}

void Character::teleport(int x, int y)
{
	icoord dest = getTileCoords_i() + icoord(x, y, 0);
	if (canMove(dest))
		setTileCoords(dest);
}

void Character::move(int x, int y)
{
	moveByTile(x, y);
}

void Character::erase()
{
	area->erase(this);
}

