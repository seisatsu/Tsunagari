/****************************
** Tsunagari Tile Engine   **
** sprite.h - Sprite       **
** Copyright 2011 OmegaSDG **
****************************/

#ifndef SPRITE_H
#define SPRITE_H

#include <string>

#include <Gosu/Gosu.hpp>

#include "resourcer.h"

class Resourcer;

class Sprite
{
public:
	Sprite(Resourcer* rc, std::wstring img_fn);
	void draw();

private:
	Gosu::Image* img;
};

#endif

