/******************************
** Tsunagari Tile Engine     **
** entity.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include <boost/scoped_ptr.hpp>

#include "common.h"
#include "sprite.h"

class Area;
class Resourcer;

//! Entity Class
/*!
	This class handles dynamic game objects, such as monsters, NPCs, and
	items.
*/
class Entity
{
public:
	//! Entity Constructor
	Entity(Resourcer* rc, Area* area, const std::string& descriptor);
	
	//! Entity Destructor
	virtual ~Entity();
	
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
	virtual void moveByTile(coord_t delta);

	//! Set location to Tile at {x, y, z}.
	void setCoordsByTile(coord_t pos);

	//! Sets the Area object this entity will ask when looking for
	// nearby Tiles. Doesn't change x,y,z position.
	void setArea(Area* area);

protected:
	bool processDescriptor();
	bool processPlayerDescriptor(const xmlNode* root);
	
	virtual void postMove();

	Resourcer* rc;
	Sprite sprite;
	Area* area;
	bool redraw;
	
	enum entityType {
		PLAYER
	};
	
	const std::string descriptor;
	std::string spriteDescriptor;
	entityType type;
};

#endif

