/******************************
** Tsunagari Tile Engine     **
** entity.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include "common.h"

class Area;
class Resourcer;
class Sprite;

//! Entity Class
/*!
	This class handles dynamic game objects, such as monsters, NPCs, and
	items.
*/
class Entity
{
public:
	//! Entity Constructor
	Entity(Resourcer* rc, Area* area, const std::string descriptor,
		const std::string spriteDescriptor);
	
	//! Entity Destructor
	~Entity();
	
	//! Entity Initializer
	bool init();
	
	//! Gosu Callback
	void draw();
	
	//! Gosu Callback
	bool needsRedraw() const;
	
	//! Retrieve entity's absolute position.
	coord_t getCoordsByPixel();
	
	//! Retrieve entity's tile position.
	coord_t getCoordsByTile();

	//! Move the entity by dx, dy.
	void moveByTile(coord_t delta);

	//! Set location to Tile at {x, y, z}.
	void setCoordsByTile(coord_t pos);

	//! Sets the Area object this entity will ask when looking for
	// nearby Tiles. Doesn't change x,y,z position.
	void setArea(Area* area);

protected:
	void postMove();

	Resourcer* rc;
	Sprite* sprite;
	Area* area; // XXX init
	bool redraw;

	const std::string descriptor;
	const std::string spriteDescriptor;
};

#endif

