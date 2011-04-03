/******************************
** Tsunagari Tile Engine     **
** sprite.cpp - Sprite       **
** Copyright 2011 OmegaSDG   **
******************************/

#include "sprite.h"

Sprite::Sprite(Resourcer* rc, std::string img_fn)
{
	img = rc->get_image(img_fn);
	c.x = c.y = 0;
}

Sprite::~Sprite()
{
	delete img;
}

void Sprite::draw()
{
	img->draw(c.x, c.y, 0);
}

