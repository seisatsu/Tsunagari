/******************************
** Tsunagari Tile Engine     **
** world.h - World           **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include <string>
#include <Gosu/Gosu.hpp>

#include "area.h"
#include "common.h"
#include "entity.h"
#include "resourcer.h"
#include "window.h"

class Area;
class Entity;
class GameWindow;
class Resourcer;

enum WorldType {
	LOCAL,
	NETWORK
};

struct WorldEntry {
	std::string area;
	coord_t* coords;
};

struct WorldValues {
	std::string name;
	std::string author;
	std::string playersprite;
	coord_t* tilesize;
	WorldType type;
	WorldEntry* entry;
};

class World
{
public:
	World(GameWindow* window, Resourcer* rc);
	~World();

	bool init(std::string descriptor);
	void button_down(Gosu::Button btn);
	void draw();
	bool needs_redraw();

/*
	void button_down(int id);
	void load_area(Area* area);
*/

private:
	bool processDescriptor(std::string descriptor);
	Area* area;
	Entity* player;
	Resourcer* _rc;
	GameWindow* _window;
	Json::Value root;
	Json::Value entrypoint;
	Json::Value tilesize;
	Json::Reader reader;
	bool parsingSuccessful;
	std::string typeTemp;

	WorldValues* values; // Descriptor data
/*
	Window* window;
	Resourcer* rc;
	bool redraw;
*/

};

#endif

