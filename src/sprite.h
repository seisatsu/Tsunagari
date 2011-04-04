/******************************
** Tsunagari Tile Engine     **
** sprite.h - Sprite         **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef SPRITE_H
#define SPRITE_H

#include <string>

#include <Gosu/Gosu.hpp>

#include "common.h"
#include "resourcer.h"


class Resourcer;

/**
 * Sprite represents a comprehensive model for displaying 2D video game images.
 *
 * Each Sprite represents one 'thing' that will be rendered to the screen.
 * Sprite can handle animated images that cycle through their frames over time.
 * It also has the capacity to switch between a couple different images on
 * demand.
 *
 * For example, you might have a Sprite for a player character which had
 * animated models for each possible movement direction (up, down, left, right)
 * along with static standing-still images for each direction.
 * This would all be handled by one Sprite.
 */
class Sprite
{
public:
	Sprite(Resourcer* rc, std::string img_fn);
	~Sprite();

	void draw();

	void move(int dx, int dy);

private:
	Gosu::Image* img;
	coord_t c;
};

#endif

