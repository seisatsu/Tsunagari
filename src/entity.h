/******************************
** Tsunagari Tile Engine     **
** sprite.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef SPRITE_H
#define SPRITE_H

#include <string>

#include <boost/scoped_ptr.hpp>
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

//! Entity Class
/*!
	This class handles dynamic game objects, such as monsters, NPCs, and
	items.
*/

/**
 * Sprite is an image model for displaying 2D video game entities.
 *
 * Each Sprite represents one 'thing' that will be rendered to the screen.
 * Sprite can handle animated images that cycle through their frames over time.
 * It also has the capacity to switch between a couple different images on
 * demand.
 *
 * For example, you might have a Sprite for a player character with animated
 * models for walking in each possible movement direction (up, down, left,
 * right) along with static standing-still images for each direction. This
 * could all be handled by one Sprite.
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
	bool processDescriptor();
	bool processSprite(const xmlNode* sprite);
	bool processPhases(const xmlNode* phases);
	bool processPhase(xmlNode* phase);
	bool processSounds(const xmlNode* sounds);
	bool processSound(xmlNode* sound);

	bool loadPhases();
	Gosu::Image* loadImage(const Gosu::Bitmap& src, unsigned pos);

	virtual void postMove();


	Resourcer* rc;

	boost::unordered_map<std::string, SampleRef> sounds;
	boost::unordered_map<std::string, Gosu::Image*> imgs;
	Gosu::Image* img;
	bool redraw;

	Area* area;
	coord_t c;

	std::string descriptor;

	//! SpriteValues XML Storage Struct
	/*!
		Main XML storage struct for Sprite.
	*/
	struct SpriteValues {
		std::string sheet;
		coord_t tileSize; // z-coord not used
		boost::unordered_map<std::string, unsigned> phases;
	} xml;
};

#endif

