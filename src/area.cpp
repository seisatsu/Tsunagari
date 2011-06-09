/******************************
** Tsunagari Tile Engine     **
** area.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

<<<<<<< HEAD
#include <boost/foreach.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

=======
>>>>>>> origin/master
#include "area.h"
#include "entity.h"
#include "resourcer.h"
#include "sprite.h"
#include "tile.h"

Area::Area(Resourcer* rc, Entity* player, const std::string descriptor)
	: rc(rc), player(player), descriptor(descriptor)
{
}

Area::~Area()
{
}

bool Area::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return false;
	
	Sprite* s = new Sprite(rc, "grass.sprite");
	Sprite* s2 = new Sprite(rc, "grass.sprite");
	if (!s->init() || !s2->init())
		return false;
	Tile* t = new Tile(s, true, coord(0, 0, 0));
	Tile* t2 = new Tile(s2, true, coord(1, 0, 0));


	matrix.resize(1);
	matrix[0].resize(1);
	matrix[0][0].resize(2);
	matrix[0][0][0] = t;
	matrix[0][0][1] = t2;
	
	return true;
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
	BOOST_FOREACH(grid_t g, matrix)
		BOOST_FOREACH(row_t r, g)
			BOOST_FOREACH(Tile* t, r)
				t->draw();
	player->draw();
}

bool Area::needsRedraw() const
{
	return player->needsRedraw();
}

bool Area::processDescriptor()
{
	xmlDoc* doc = rc->getXMLDoc(descriptor);
	if (!doc)
		return false;
	
	//! Push root element onto the stack.
	xmlStack.push(xmlDocGetRootElement(doc));
	
	if (!xmlStack.top()) {
		xmlFreeDoc(doc);
		return false;
	}
	
	xmlStack.push(xmlStack.top()->xmlChildrenNode); // <area>
	
	xmlFreeDoc(doc);
	return true;
}

coord_t Area::getDimensions()
{
	return dim;
}

Tile* Area::getTile(coord_t c)
{
	return matrix[c.z][c.y][c.x];
}

