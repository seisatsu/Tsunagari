/******************************
** Tsunagari Tile Engine     **
** world.h                   **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef WORLD_H
#define WORLD_H

#include <string>

#include <Gosu/Gosu.hpp>

#include "area.h"
#include "common.h"
#include "entity.h"
#include "resourcer.h"

class Area;
class Entity;
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
	World(Resourcer* rc, const std::string descriptor);
	~World();

	bool init();
	void buttonDown(const Gosu::Button btn);
	void draw();
	bool needsRedraw() const;

/*
	void loadArea(Area* area);
*/

private:
	bool processDescriptor();

	Resourcer* rc;
	Area* area;
	Entity* player;

	const std::string descriptor;
	WorldValues values; // Descriptor data
};

#endif

