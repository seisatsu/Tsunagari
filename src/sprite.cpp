/******************************
** Tsunagari Tile Engine     **
** sprite.cpp - Sprite       **
** Copyright 2011 OmegaSDG   **
******************************/

#include "sprite.h"

Sprite::Sprite(Resourcer* rc, std::wstring img_fn)
	: x(0), y(0)
{
	img = rc->get_image(img_fn);
}

void Sprite::draw()
{
	img->draw(x, y, 0);
}

