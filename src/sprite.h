/******************************
** Tsunagari Tile Engine     **
** sprite.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef SPRITE_H
#define SPRITE_H

#include <map>
#include <stdint.h>
#include <string>

#include <Gosu/Gosu.hpp>

#include "common.h"

class Resourcer;

//! SpriteValues JSON Storage Struct
/*!
	Main JSON storage struct for Sprite.
*/
struct SpriteValues {
	std::string sheet;
	coord_t tilesize;
	std::map<std::string, uint32_t> phases;
};

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
 * would all be handled by one Sprite.
 */
class Sprite
{
public:
	//! Sprite Constructor
	Sprite(Resourcer* rc, const std::string descriptor);
	
	//! Sprite Destructor
	~Sprite();
	
	//! Sprite Initializer
	bool init();
	
	//! Gosu Callback
	void draw() const;
	
	//! Gosu Callback
	void move(int dx, int dy);

private:
	bool processDescriptor();

	Resourcer* rc;
	const Gosu::Image* img;
	coord_t c;

	const std::string descriptor;
	SpriteValues values; // Descriptor data
};

#endif

