/******************************
** Tsunagari Tile Engine     **
** world.cpp                 **
** Copyright 2011 OmegaSDG   **
******************************/

#include "world.h"

World::World(GameWindow* window, Resourcer* rc, const std::string descriptor)
	: window(window), rc(rc), descriptor(descriptor)
{
}

World::~World()
{
	if (player)
		delete player;
	if (area)
		delete area;
}

bool World::processDescriptor()
{
	Json::Reader reader;
	Json::Value root;
	Json::Value entrypoint;
	Json::Value tilesize;

	std::ifstream file(descriptor.c_str());

	// Here, we load in the world descriptor file. It's a little messy.
	if (!reader.parse(file, root)) {
		Log::err(descriptor, "File missing.");
		return false;
	}

	/* Extract from JSON object:
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

	tilesize = root["tilesize"];
	if (tilesize.size() != 2) {
		Log::err(descriptor, ": \"tilesize\" [2] required.\n");
		return false;
	}
	values.tilesize.x = tilesize[0u].asUInt();
	values.tilesize.y = tilesize[1].asUInt();

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

bool World::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return false;

	player = new Entity(rc, "_NONE_", values.playersprite); // The player entity doesn't have a descriptor yet.
	if (!player->init())
		return false;

	area = new Area(window, rc, player, values.entry.area);

	return true;
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

