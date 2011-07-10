/******************************
** Tsunagari Tile Engine     **
** entity.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include "area.h"
#include "entity.h"
#include "sprite.h"
#include "world.h"

Entity::Entity(Resourcer* rc,
               Area* area,
               const std::string& descriptor,
               const std::string& spriteDescriptor)
	: rc(rc),
	  sprite(rc, spriteDescriptor),
	  area(area),
	  redraw(true),
	  descriptor(descriptor)
{
}

Entity::~Entity()
{
}

bool Entity::init()
{
	return sprite.init();
}

void Entity::draw()
{
	redraw = false;
	sprite.draw();
}

bool Entity::needsRedraw() const
{
	return redraw;
}

coord_t Entity::getCoordsByPixel()
{
	return sprite.getCoordsByPixel();
}

coord_t Entity::getCoordsByTile()
{
	return sprite.getCoordsByTile();
}

void Entity::moveByTile(coord_t delta)
{
	coord_t newCoord = sprite.getCoordsByTile();
	newCoord.x += delta.x;
	newCoord.y += delta.y;
	newCoord.z += delta.z;
	Area::Tile* dest = area->getTile(newCoord);
	if ((dest->flags       & Area::nowalk) != 0 ||
	    (dest->type->flags & Area::nowalk) != 0) {
		// The tile we're trying to move onto is set as nowalk.
		// Stop here.
		return;
	}
	sprite.moveByTile(delta);
	redraw = true;
	postMove();
}

void Entity::setCoordsByTile(coord_t pos)
{
	sprite.setCoordsByTile(pos);
	redraw = true;
}

void Entity::setArea(Area* area)
{
	this->area = area;
}

void Entity::postMove()
{
}

