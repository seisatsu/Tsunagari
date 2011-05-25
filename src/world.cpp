/******************************
** Tsunagari Tile Engine     **
** world.cpp                 **
** Copyright 2011 OmegaSDG   **
******************************/

#include <fstream>
#include <istream>

#include <json/json.h>

#include "area.h"
#include "entity.h"
#include "log.h"
#include "resourcer.h"
#include "world.h"

World::World(Resourcer* rc, GameWindow* wnd)
	: rc(rc), wnd(wnd), area(NULL), player(NULL)
{
}

World::~World()
{
	delete player;
	delete area;
}

bool World::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return false;

	// The player entity doesn't have a descriptor yet.
	player = new Entity(rc, "_NONE_", values.playersprite);
	if (!player->init())
		return false;

	area = new Area(rc, player, values.entry.area);
	
	wnd->setCaption(Gosu::widen(values.name));
	return area->init();
}

void World::buttonDown(const Gosu::Button btn)
{
	area->buttonDown(btn);
}

void World::draw()
{
	area->draw();
}

bool World::needsRedraw() const
{
	return area->needsRedraw();
}

bool World::processDescriptor()
{
	static const std::string descriptor = "world.conf";

	Json::Value root;
	Json::Value entrypoint;
	Json::Value tilesize;

	root = rc->getDescriptor(descriptor);
	if (root.empty())
		return false;

	/* Extract from XML object:
	 *  - proper name and author of World
	 *  - Sprite id to be used for the player
	 *  - whether the world supports/requires multiplayer
	 *  - width/height (in pixels) of tiles in this World
	 *  - Area id and X, Y, Z coords where players start
	 */
	values.name = root["name"].asString();
	if (values.name.empty()) {
		Log::err(descriptor, "\"name\" required.\n");
		return false;
	}

	values.author = root["author"].asString();
	if (values.author.empty()) {
		Log::err(descriptor, "\"author\" required.\n");
		return false;
	}

	values.playersprite = root["playersprite"].asString();
	if (values.playersprite.empty()) {
		Log::err(descriptor, "\"playersprite\" required.\n");
		return false;
	}

	std::string typeStr = root["type"].asString();
	if (typeStr == "local")
		values.type = LOCAL;
	else if (typeStr == "network")
		values.type = NETWORK;
	else {
		Log::err(descriptor, "\"type\" (local|network) required.\n");
		return false;
	}

	entrypoint = root["entrypoint"];
	if (entrypoint.size() != 4) {
		Log::err(descriptor, "\"entrypoint\" [4] required.\n");
		return false;
	}

	values.entry.area = entrypoint[0u].asString();
	values.entry.coords.x = entrypoint[1].asUInt();
	values.entry.coords.y = entrypoint[2].asUInt();
	values.entry.coords.z = entrypoint[3].asUInt();

	return true;
}

