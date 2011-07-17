/******************************
** Tsunagari Tile Engine     **
** entity.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include <boost/unordered_map.hpp>
#include <libxml/parser.h>

#include "common.h"
#include "resourcer.h"

class Area;
class Resourcer;

namespace Gosu {
	class Bitmap;
	class Image;
}

//! An Entity represents one 'thing' that will be rendered to the screen.
/*!
	An Entity might be a dynamic game object such as a monster, NPC, or
	item.  Entity can handle animated images that cycle through their
	frames over time. It also has the capacity to switch between a couple
	different images on demand.

	For example, you might have a Entity for a player character with
	animated models for walking in each possible movement direction (up,
	down, left, right) along with static standing-still images for each
	direction.
*/
class Entity
{
public:
	Entity(Resourcer* rc, Area* area);
	~Entity();

	//! Entity Initializer
	bool init(const std::string& descriptor);

	//! Gosu Callback
	void draw();
	bool needsRedraw() const;

	void update(unsigned long dt);

	//! Change the graphic. Returns true if it was changed to something
	//  different.
	bool setPhase(const std::string& name);

	//! Retrieve position within Area.
	coord_t getCoordsByPixel() const;
	coord_t getCoordsByTile() const;

	//! Set location within Area.
	void setCoordsByPixel(coord_t c);
	void setCoordsByTile(coord_t c);

	//! Move within Area.
	void moveByPixel(coord_t deltac);
	void moveByTile(coord_t deltac);

	//! Sets the Area object this entity will ask when looking for
	//  nearby Tiles. Doesn't change x,y,z position.
	void setArea(Area* area);
	
protected:
	SampleRef getSound(const std::string& name);

	bool processDescriptor();
	bool processSprite(const xmlNode* sprite);
	bool processPhases(const xmlNode* phases);
	bool processPhase(xmlNode* phase, const TiledImage& tiles);
	bool processMember(xmlNode* phase, Animation& anim,
                           const TiledImage& tiles);
	bool processSounds(const xmlNode* sounds);
	bool processSound(xmlNode* sound);

	//! Called right before starting to moving onto another tile.
	virtual void preMove(coord_t delta);

	//! Called after we've arrived at another tile.
	virtual void postMove();


	Resourcer* rc;

	boost::unordered_map<std::string, Animation> phases;
	Animation* phase;
	bool redraw;

	boost::unordered_map<std::string, SampleRef> sounds;

	bool moving;
	coord_t dest;
	double speed;

	Area* area;
	coord_t c;
	double rx, ry, rz; // real x,y position: hold partial pixel transversal

	std::string descriptor;

	//! SpriteValues XML Storage Struct
	/*!
		Main XML storage struct for Sprite.
	*/
	struct SpriteValues {
		std::string sheet;
		coord_t tileSize;
		boost::unordered_map<std::string, unsigned> phases;
	} xml;
};

#endif

