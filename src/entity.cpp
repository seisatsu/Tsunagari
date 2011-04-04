/******************************
** Tsunagari Tile Engine     **
** entity.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#define TILE_SIZE 64

#include "entity.h"

Entity::Entity(Resourcer* rc, std::string img_fn)
	: redraw(false)
{
	sprite = new Sprite(rc, img_fn);
}

Entity::~Entity()
{
	delete sprite;
}

void Entity::draw()
{
	redraw = false;
	sprite->draw();
}

bool Entity::needs_redraw()
{
	return redraw;
}

void Entity::move(int dx, int dy)
{
	sprite->move(dx*TILE_SIZE, dy*TILE_SIZE);
}

