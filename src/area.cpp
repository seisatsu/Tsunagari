/******************************
** Tsunagari Tile Engine     **
** area.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <stdio.h>

#include "area.h"

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
	
	tiles = new TileMatrix(rc);
	
	return tiles->init();
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
	tiles->draw();
	player->draw();
}

bool Area::needsRedraw() const
{
	return player->needsRedraw();
}

bool Area::processDescriptor()
{
/*	xmlChar* str;
	
	xmlNode* root = rc->getXMLDoc(descriptor);
	if (!root)
		return false;
	
	xmlNode* node = root->xmlChildrenNode; // <area>
	node = node->xmlChildrenNode; // decend into children of <area>
	while (node != NULL) {*/
	return true;
}
