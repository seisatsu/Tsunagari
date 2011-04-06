/******************************
** Tsunagari Tile Engine     **
** world.cpp                 **
** Copyright 2011 OmegaSDG   **
******************************/

#include "world.h"

World::World(GameWindow* window, Resourcer* rc)
{
	// Looks just a bit cleaner to the other classes.
	_window = window;
	_rc = rc;
	player = new Entity(rc, "../testing/outpost_world/player.sheet");
	area = new Area(window, rc, player, "../testing/babysfirst.area");
}

World::~World()
{
	delete player;
	delete area;
}

bool World::init(std::string descriptor)
{	
	/* Paul: Get this working. I can't do it without breaking everything.
	   The way you have things set up, everything is finished starting
	   the moment the first constructor is called. There's no room to call
	   any init functions.
	*/
	
	// Parse JSON descriptor. This example can be used for other classes.
	player = new Entity(_rc, "../testing/outpost_world/player.sheet");
	parsingSuccessful = reader.parse(descriptor, root);
	if (!parsingSuccessful)
		return false;
	
	values->name = root.get("name", "_NONE_").asString();
	if (values->name.compare("_NONE_") == 0)
		return false;
	
	values->author = root.get("author", "_NONE_").asString();
	if (values->author.compare("_NONE_") == 0)
		return false;
	
	typeTemp = root.get("type", "_NONE_").asString();
	if (typeTemp.compare("lower") == 0)
		values->type = LOCAL;
	else if (typeTemp.compare("network") == 0)
		values->type = NETWORK;
	else
		return false;
	
	entrypoint = root["entrypoint"];
	if (entrypoint.size() != 4)
		return false;
	values->entry->area = entrypoint[uint(0)].asString(); // I don't understand why I have to do this.
	values->entry->coords->x = entrypoint[1].asInt();
	values->entry->coords->y = entrypoint[2].asInt();
	values->entry->coords->z = entrypoint[3].asInt();
	
	area = new Area(_window, _rc, player, values->entry->area);
	
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

