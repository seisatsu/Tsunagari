/******************************
** Tsunagari Tile Engine     **
** area.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <stack>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "area.h"
#include "entity.h"
#include "resourcer.h"
#include "sprite.h"

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
	
	Gosu::Image* grassImg = rc->getImage("grass.sheet");
	TileType* grassTile = new TileType;
	grassTile->graphics.push_back(grassImg);
	grassTile->animated = false;
	Tile* t = new Tile;
	Tile* t2 = new Tile;
	t->type = t2->type = grassTile;

	map.resize(1);
	map[0].resize(1);
	map[0][0].resize(2);
	map[0][0][0] = t;
	map[0][0][1] = t2;
	
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
	for (unsigned int layer = 0; layer != map.size(); layer++)
	{
		grid_t grid = map[layer];
		for (unsigned int y = 0; y != grid.size(); y++)
		{
			row_t row = grid[layer];
			for (unsigned int x = 0; x != row.size(); x++)
			{
				Tile* tile = row[x];
				Gosu::Image* img = tile->type->graphics[0];
				img->draw(x*img->width(), y*img->height(), 0);
			}
		}
	}
	player->draw();
}

bool Area::needsRedraw() const
{
	return player->needsRedraw();
}

bool Area::processDescriptor()
{
	//! Parser node stack
	std::stack<xmlNode*> xmlStack;
	
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

Area::Tile* Area::getTile(coord_t c)
{
	return map[c.z][c.y][c.x];
}

