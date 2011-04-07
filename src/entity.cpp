/******************************
** Tsunagari Tile Engine     **
** entity.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#define TILE_SIZE 64

#include "entity.h"

Entity::Entity(Resourcer* rc, const std::string descriptor, const std::string sprite_descriptor) : redraw(false)
{
	this->rc = rc;
	this->descriptor = descriptor;
	this->sprite_descriptor = sprite_descriptor;
}

Entity::~Entity()
{
	delete sprite;
}

int Entity::init()
{
	sprite = new Sprite(rc, sprite_descriptor);
	return sprite->init();
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

