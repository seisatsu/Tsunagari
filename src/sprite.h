/******************************
** Tsunagari Tile Engine     **
** sprite.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef SPRITE_H
#define SPRITE_H

#include <string>

#include <boost/unordered_map.hpp>
#include <libxml/parser.h>

#include "common.h"
#include "resourcer.h"

namespace Gosu {
	class Bitmap;
	class Image;
}

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
class Sprite
{
public:
	//! Sprite Constructor
	Sprite(Resourcer* rc);

	//! Sprite Destructor
	~Sprite();

	//! Sprite Initializer
	bool init(const std::string& descriptor);

	//! Gosu Callback
	void draw() const;

	bool setPhase(const std::string& name);

	coord_t getCoordsByPixel();
	coord_t getCoordsByTile();

	void setCoordsByPixel(coord_t c);
	void setCoordsByTile(coord_t c);
	void moveByPixel(coord_t deltac);
	void moveByTile(coord_t deltac);

private:
	bool processDescriptor();
	bool processPhases(xmlNode* phases);
	bool processPhase(xmlNode* phase);

	bool loadPhases();
	Gosu::Image* loadImage(const Gosu::Bitmap& src, unsigned pos);


	Resourcer* rc;
	boost::unordered_map<std::string, Gosu::Image*> imgs;
	Gosu::Image* img;
	coord_t c;

	std::string descriptor;

	//! SpriteValues XML Storage Struct
	/*!
	  Main XML storage struct for Sprite.
	*/
	struct SpriteValues {
		std::string sheet;
		coord_t tilesize; // z-coord not used
		boost::unordered_map<std::string, unsigned> phases;
	} xml;
};

#endif

