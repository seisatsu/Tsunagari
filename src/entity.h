/******************************
** Tsunagari Tile Engine     **
** entity.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include "common.h"

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
	Entity(Resourcer* rc, const std::string descriptor,
		const std::string spriteDescriptor);
	
	//! Entity Destructor
	~Entity();
	
	//! Entity Initializer
	bool init();
	
	//! Gosu Callback
	void draw();
	
	//! Gosu Callback
	bool needsRedraw() const;
	
	coord_t getCoordsByPixel();
	coord_t getCoordsByTile();
	
	//! Move the entity by dx, dy.
	void moveByTile(coord_t delta);

protected:
	Sprite* sprite;
	Resourcer* rc;
	bool redraw;

	const std::string descriptor;
	const std::string spriteDescriptor;
};

#endif

