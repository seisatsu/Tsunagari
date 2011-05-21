/******************************
** Tsunagari Tile Engine     **
** area.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <stdio.h>

#include "area.h"

Area::Area(Resourcer* rc, Entity* player, const std::string )
	: rc(rc), player(player)
{
}

Area::~Area()
{
}

bool Area::init()
{
	Sprite* s = new Sprite(rc, "grass.sprite");
	if (!s->init())
		return false;
	t = new Tile(s, true, {0, 0, 0});
	return true;
}

void Area::buttonDown(const Gosu::Button btn)
{
	if (btn == Gosu::kbRight)
		player->move(1, 0);
	else if (btn == Gosu::kbLeft)
		player->move(-1, 0);
	else if (btn == Gosu::kbUp)
		player->move(0, -1);
	else if (btn == Gosu::kbDown)
		player->move(0, 1);
}

void Area::draw()
{
	t->draw();
	player->draw();
}

bool Area::needsRedraw() const
{
	return player->needsRedraw();
}

