/******************************
** Tsunagari Tile Engine     **
** sprite.cpp - Sprite       **
** Copyright 2011 OmegaSDG   **
******************************/

#include "sprite.h"

Sprite::Sprite(Resourcer* rc, std::wstring img_fn)
{
	img = rc->get_image(img_fn);
}

void Sprite::draw()
{
	img->draw(0, 0, 0);
}

