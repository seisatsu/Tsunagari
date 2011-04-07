/******************************
** Tsunagari Tile Engine     **
** entity.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#define TILE_SIZE 64

#include "entity.h"

Entity::Entity(Resourcer* rc, const std::string descriptor, const std::string spriteDescriptor) : redraw(false)
{
	this->rc = rc;
	this->descriptor = descriptor;
	this->spriteDescriptor = spriteDescriptor;
}

Entity::~Entity()
{
	delete sprite;
}

int Entity::init()
{
	sprite = new Sprite(rc, spriteDescriptor);
	return sprite->init();
}

void Entity::draw()
{
	redraw = false;
	sprite->draw();
}

bool Entity::needsRedraw()
{
	return redraw;
}

void Entity::move(int dx, int dy)
{
	sprite->move(dx*TILE_SIZE, dy*TILE_SIZE);
}

