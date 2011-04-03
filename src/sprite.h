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

class Sprite
{
public:
	Sprite(Resourcer* rc, std::string img_fn);
	~Sprite();

	void draw();

private:
	Gosu::Image* img;
	coord_t c;
};

#endif

