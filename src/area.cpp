/******************************
** Tsunagari Tile Engine     **
** area.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <stdio.h>

#include "area.h"

Area::Area(Resourcer* rc, Entity* player, const std::string )
	: rc(rc), player(player), tiles(rc)
{
}

Area::~Area()
{
}

bool Area::init()
{
	return tiles.init();
}

void Area::buttonDown(const Gosu::Button btn)
{
	if (btn == Gosu::kbRight)
		player->moveByTile(coord(1, 0, 0));
	else if (btn == Gosu::kbLeft)
		player->moveByTile(coord(-1, 0, 0));
	else if (btn == Gosu::kbUp)
		player->moveByTile(coord(0, -1, 0));
	else if (btn == Gosu::kbDown)
		player->moveByTile(coord(0, 1, 0));
}

void Area::draw()
{
	tiles.draw();
	player->draw();
}

bool Area::needsRedraw() const
{
	return player->needsRedraw();
}

