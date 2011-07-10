/******************************
** Tsunagari Tile Engine     **
** entity.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "area.h"
#include "entity.h"
#include "sprite.h"
#include "world.h"

Entity::Entity(Resourcer* rc,
               Area* area,
               const std::string& descriptor)
	: rc(rc),
	  sprite(rc),
	  area(area),
	  redraw(true),
	  descriptor(descriptor)
{
}

Entity::~Entity()
{
}

/**
 * Try to load in descriptor.
 */
bool Entity::processDescriptor()
{
	xmlChar* str;
	
	XMLDocRef doc = rc->getXMLDoc(descriptor, "dtd/entity.dtd");
	if (!doc)
		return false;
	const xmlNode* root = xmlDocGetRootElement(doc.get()); // <entity>
	if (!root)
		return false;
	
	str = xmlGetProp(const_cast<xmlNode*>(root), BAD_CAST("type")); // Entity type
	if (!xmlStrncmp(str, BAD_CAST("player"), 7)) {
		type = PLAYER;
		if (!processPlayerDescriptor(root))
			return false;
	}
	else {
		Log::err(descriptor, "unknown entity type");
		return false;
	}
	return true;
}

bool Entity::processPlayerDescriptor(const xmlNode* root)
{
	xmlChar* str;
	
	xmlNode* node = root->xmlChildrenNode; // children of <entity>
	for (; node != NULL; node = node->next) {
		if (!xmlStrncmp(node->name, BAD_CAST("sprite"), 7)) {
			str = xmlNodeGetContent(node);
			spriteDescriptor = (char*)str;
		}
		//TODO: <sounds>
	}
	return true;
}

bool Entity::init()
{
	return processDescriptor() && sprite.init(spriteDescriptor);
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

