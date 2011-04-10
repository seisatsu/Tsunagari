/******************************
** Tsunagari Tile Engine     **
** entity.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef ENTITY_H
#define ENTITY_H

#include <string>

//#include "area.h"
#include "resourcer.h"
#include "sprite.h"
//#include "tilegrid.h"
//#include "world.h"

class Resourcer;
class Sprite;

class Entity
{
public:
	Entity(Resourcer* rc, const std::string descriptor,
		const std::string spriteDescriptor);
	~Entity();

	bool init();

	void draw();
	bool needsRedraw() const;

	void move(int dx, int dy);

/*
	Entity(World* world, Resource* rc);
	void setArea(Area* area);
	void goTo(Area* area, coord_t coords);
	coord_t getCoords();
	void transport(coord_t coords);
*/

protected:
	Sprite* sprite;
	Resourcer* rc;
	bool redraw;

	const std::string descriptor;
	const std::string spriteDescriptor;

/*
	World* world;
	TileGrid* grid;
*/
};

#endif

