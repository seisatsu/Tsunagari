/******************************
** Tsunagari Tile Engine     **
** world.cpp                 **
** Copyright 2011 OmegaSDG   **
******************************/

#include <iostream>

#include "world.h"

World::World(GameWindow* window, Resourcer* rc, const std::string descriptor)
{
	this->window = window;
	this->rc = rc;
	this->descriptor = descriptor;
}

World::~World()
{
	delete player;
	delete area;
}

bool World::processDescriptor()
{	
	std::ifstream file(descriptor.c_str());

	Json::Value root;
	Json::Value entrypoint;
	Json::Value tilesize;
	Json::Reader reader;

	// Here we load in the world descriptor file. It's a little messy.
	if (!reader.parse(file, root)) // Actual parsing.
		return false;

	// Begin loading in configuration values.
	values.name = root.get("name", "_NONE_").asString(); // name
	if (values.name.compare("_NONE_") == 0) {
		std::cerr << "Error: " << descriptor << ": \"name\" required.\n";
		return false;
	}

	values.author = root.get("author", "_NONE_").asString(); // author
	if (values.author.compare("_NONE_") == 0) {
		std::cerr << "Error: " << descriptor << ": \"author\" required.\n";
		return false;
	}

	values.playersprite = root.get("playersprite", "_NONE_").asString(); // playersprite
	if (values.playersprite.compare("_NONE_") == 0) {
		std::cerr << "Error: " << descriptor << ": \"playersprite\" required.\n";
		return false;
	}

	typeTemp = root.get("type", "_NONE_").asString(); // type
	if (typeTemp.compare("local") == 0)
		values.type = LOCAL;
	else if (typeTemp.compare("network") == 0)
		values.type = NETWORK;
	else {
		std::cerr << "Error: " << descriptor << ": \"type\" (local|network) required.\n";
		return false;
	}

	tilesize = root["tilesize"]; // tilesize
	if (tilesize.size() != 2) {
		std::cerr << "Error: " << descriptor << ": \"tilesize\" [2] required.\n";
		return false;
	}
	values.tilesize.x = tilesize[uint(0)].asUInt(); // I don't understand why I have to do this.
	values.tilesize.y = tilesize[1].asUInt();

	entrypoint = root["entrypoint"]; // entrypoint
	if (entrypoint.size() != 4) {
		std::cerr << "Error: " << descriptor << ": \"entrypoint\" [4] required.\n";
		return false;
	}
	values.entry.area = entrypoint[uint(0)].asString(); // Same thing here. The compiler assumes 0 is a signed int.
	values.entry.coords.x = entrypoint[1].asUInt();
	values.entry.coords.y = entrypoint[2].asUInt();
	values.entry.coords.z = entrypoint[3].asUInt();

	file.close();
	return true;
}

int World::init()
{
	// Initialization
	int entityReturnValue;

	if (!processDescriptor()) // Try to load in descriptor.
		return 2;

	player = new Entity(rc, "_NONE_", values.playersprite); // The player entity doesn't have a descriptor yet.
	entityReturnValue = player->init();
	if (entityReturnValue != 0)
		return entityReturnValue;

	area = new Area(window, rc, player, values.entry.area);

	return 0;
}

void World::buttonDown(const Gosu::Button btn)
{
	area->buttonDown(btn);
}

void World::draw()
{
	area->draw();
}

bool World::needsRedraw()
{
	return area->needsRedraw();
}

