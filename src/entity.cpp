/******************************
** Tsunagari Tile Engine     **
** entity.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include "entity.h"

Entity::Entity(Resourcer* rc, std::wstring img_fn)
{
	sprite = new Sprite(rc, img_fn);
}

void Entity::draw()
{
	sprite->draw();
}

