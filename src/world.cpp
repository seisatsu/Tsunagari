/******************************
** Tsunagari Tile Engine     **
** world.cpp                 **
** Copyright 2011 OmegaSDG   **
******************************/

#include <fstream>
#include <istream>

#include "world.h"

World::World(GameWindow* window, Resourcer* rc)
{
	// Looks just a bit cleaner to the other classes.
	_window = window;
	_rc = rc;
	values = new WorldValues;
	values->entry = new WorldEntry;
	values->entry->coords = new coord_t;
}

World::~World()
{
	delete player;
	delete area;
	delete values->entry->coords;
	delete values->entry;
	delete values;
}

bool World::processDescriptor(std::string descriptor)
{	
	std::ifstream file(descriptor.c_str());
	
	// Here we load in the world descriptor file. It's a little messy.
	parsingSuccessful = reader.parse(file, root); // Actual parsing.
	if (!parsingSuccessful)
		return false;
	
	// Begin loading in configuration values.
	values->name = root.get("name", "_NONE_").asString(); // name
	if (values->name.compare("_NONE_") == 0)
		return false;
	
	values->author = root.get("author", "_NONE_").asString(); // author
	if (values->author.compare("_NONE_") == 0)
		return false;
	
	values->playersprite = root.get("playersprite", "_NONE_").asString(); // playersprite
	if (values->playersprite.compare("_NONE_") == 0)
		return false;
	
	typeTemp = root.get("type", "_NONE_").asString(); // type
	if (typeTemp.compare("lower") == 0)
		values->type = LOCAL;
	else if (typeTemp.compare("network") == 0)
		values->type = NETWORK;
	else
		return false;
	
	tilesize = root["tilesize"]; // tilesize
	if (tilesize.size() != 2)
		return false;
	values->tilesize->x = tilesize[uint(0)].asUInt(); // I don't understand why I have to do this.
	values->tilesize->y = tilesize[1].asUInt();
	
	entrypoint = root["entrypoint"]; // entrypoint
	if (entrypoint.size() != 4)
		return false;
	values->entry->area = entrypoint[uint(0)].asString(); // Same thing here. The compiler assumes 0 is a signed int.
	values->entry->coords->x = entrypoint[1].asUInt();
	values->entry->coords->y = entrypoint[2].asUInt();
	values->entry->coords->z = entrypoint[3].asUInt();
	
	file.close();
	return true;
}

bool World::init(std::string descriptor)
{	
	// Initialization
	if (!processDescriptor(descriptor)) // Try to load in descriptor.
		return false;
	
	area = new Area(_window, _rc, player, values->entry->area);
	player = new Entity(_rc, values->playersprite);
	
	return true;
}

void World::button_down(Gosu::Button btn)
{
	area->button_down(btn);
}

void World::draw()
{
	area->draw();
}

bool World::needs_redraw()
{
	return area->needs_redraw();
}

