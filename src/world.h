/******************************
** Tsunagari Tile Engine     **
** world.h                   **
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
	coord_t coords;
};

struct WorldValues {
	std::string name;
	std::string author;
	std::string playersprite;
	coord_t tilesize;
	WorldType type;
	WorldEntry entry;
};

class World
{
public:
	World(GameWindow* window, Resourcer* rc, const std::string descriptor);
	~World();

	int init();
	void buttonDown(const Gosu::Button btn);
	void draw();
	bool needsRedraw() const;

/*
	void loadArea(Area* area);
*/

private:
	bool processDescriptor();

	GameWindow* window;
	Resourcer* rc;
	Area* area;
	Entity* player;

	const std::string descriptor;
	WorldValues values; // Descriptor data
};

#endif

