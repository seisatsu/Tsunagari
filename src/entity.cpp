/******************************
** Tsunagari Tile Engine     **
** entity.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Image.hpp>
#include <Gosu/Math.hpp>
#include <Gosu/Timing.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "area.h"
#include "config.h"
#include "entity.h"
#include "log.h"
#include "resourcer.h"

Entity::Entity(Resourcer* rc, Area* area)
	: rc(rc),
	  redraw(true),
	  moving(false),
	  speed(240.0 / 1000),
	  area(area)
{
	c.x = c.y = c.z = 0;
}

Entity::~Entity()
{
}

bool Entity::init(const std::string& descriptor)
{
	this->descriptor = descriptor;
	if (!processDescriptor())
		return false;

	// Set an initial phase
	boost::unordered_map<std::string, Animation>::iterator it;
	it = phases.begin();
	phase = &it->second;

	return true;
}

void Entity::draw()
{
	int millis = (int)Gosu::milliseconds();
	phase->updateFrame(millis);
	phase->frame()->draw((double)c.x, (double)c.y, (double)0);
	redraw = false;
}

bool Entity::needsRedraw() const
{
	int millis = (int)Gosu::milliseconds();
	return redraw || phase->needsRedraw(millis);
}

void Entity::update(unsigned long dt)
{
	if (GAME_MODE == SLIDE_MOVE && moving) {
		redraw = true;

		double destDist = Gosu::distance((double)c.x, (double)c.y,
				(double)dest.x, (double)dest.y); 
		if (destDist < speed * (double)dt) {
			c = dest;
			moving = false;
			postMove();
		}
		else {
			double angle = Gosu::angle((double)c.x, (double)c.y,
					(double)dest.x, (double)dest.y); 
			double dx = Gosu::offsetX(angle, speed * (double)dt);
			double dy = Gosu::offsetY(angle, speed * (double)dt);

			// Save state of partial pixels traveled in double
			rx += dx;
			ry += dy;

			c.x = (long)rx;
			c.y = (long)ry;
		}
	}
}

bool Entity::setPhase(const std::string& name)
{
	bool changed = false;
	boost::unordered_map<std::string, Animation>::iterator it;
	it = phases.find(name);
	if (it != phases.end()) {
		Animation* newPhase = &it->second;
		changed = (phase != newPhase);
		phase = newPhase;
	}
	return changed;
}

coord_t Entity::getCoordsByPixel() const
{
	return c;
}

coord_t Entity::getCoordsByTile() const
{
	coord_t tileDim = area->getTileDimensions();
	coord_t coords;
	coords.x = c.x / tileDim.x;
	coords.y = c.y / tileDim.y;
	coords.z = c.z; // XXX: revisit when we have Z-buffers
	return coords;
}

void Entity::setCoordsByPixel(coord_t coords)
{
	c = coords;
	redraw = true;
}

void Entity::setCoordsByTile(coord_t coords)
{
	coord_t tileDim = area->getTileDimensions();
	c = coords;
	c.x *= tileDim.x;
	c.y *= tileDim.y;
	// XXX: set c.z when we have Z-buffers
	redraw = true;
}

void Entity::moveByPixel(coord_t dc)
{
	c.x += dc.x;
	c.y += dc.y;
	c.z += dc.z;
	redraw = true;
}

void Entity::moveByTile(coord_t dc)
{
	if (GAME_MODE == SLIDE_MOVE && moving)
		// support queueing moves?
		return;

	coord_t newCoord = getCoordsByTile();
	newCoord.x += dc.x;
	newCoord.y += dc.y;
	newCoord.z += dc.z;

	// Can we move?
	const Area::Tile& tile = area->getTile(newCoord);
	if ((tile.flags       & Area::nowalk) != 0 ||
	    (tile.type->flags & Area::nowalk) != 0) {
		// The tile we're trying to move onto is set as nowalk.
		// Stop here.
		return;
	}

	// Move!
	redraw = true;
	const coord_t tileDim = area->getTileDimensions();
	dest.x = c.x + dc.x * tileDim.x;
	dest.y = c.y + dc.y * tileDim.y;
	dest.z = 0; // XXX: set dest.z when we have Z-buffers

	preMove(dc);

	if (GAME_MODE == JUMP_MOVE) {
		c.x = dest.x;
		c.y = dest.y;
		// XXX: set c.z when we have Z-buffers
		postMove();
	}
	else if (GAME_MODE == SLIDE_MOVE) {
		moving = true;
		rx = (double)c.x;
		ry = (double)c.y;
	}
}

void Entity::setArea(Area* a)
{
	area = a;
}

SampleRef Entity::getSound(const std::string& name)
{
	boost::unordered_map<std::string, SampleRef>::iterator it;
	
	it = sounds.find(name);
	if (it != sounds.end())
		return it->second;
	else
		return SampleRef();
}

/**
 * Try to load in descriptor.
 */
bool Entity::processDescriptor()
{
	XMLDocRef doc = rc->getXMLDoc(descriptor, "dtd/entity.dtd");
	if (!doc)
		return false;
	const xmlNode* root = xmlDocGetRootElement(doc.get()); // <entity>
	if (!root)
		return false;
	xmlNode* node = root->xmlChildrenNode; // children of <entity>

	for (; node != NULL; node = node->next) {
		if (!xmlStrncmp(node->name, BAD_CAST("sprite"), 6)) {
			if (!processSprite(node))
				return false;
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("sounds"), 7)) {
			if (!processSounds(node))
				return false;
		}
	}
	return true;
}

bool Entity::processSprite(const xmlNode* sprite)
{
	xmlChar* str;
	for (xmlNode* child = sprite->xmlChildrenNode; child != NULL;
			child = child->next) {
		if (!xmlStrncmp(child->name, BAD_CAST("sheet"), 6)) {
			str = xmlNodeGetContent(child);
			xml.sheet = (char*)str;

			str = xmlGetProp(child, BAD_CAST("tilewidth"));
			xml.tileSize.x = atol((char*)str); // atol

			str = xmlGetProp(child, BAD_CAST("tileheight"));
			xml.tileSize.y = atol((char*)str); // atol
		}
		else if (!xmlStrncmp(child->name, BAD_CAST("phases"), 7) &&
				!processPhases(child))
			return false;
	}
	return true;
}

bool Entity::processPhases(const xmlNode* phases)
{
	TiledImage tiles;
	if (!rc->getTiledImage(tiles, xml.sheet, (unsigned)xml.tileSize.x,
			(unsigned)xml.tileSize.y, false))
		return false;

	for (xmlNode* phase = phases->xmlChildrenNode; phase != NULL;
			phase = phase->next)
		if (!xmlStrncmp(phase->name, BAD_CAST("phase"), 6)) // needed?
			if (!processPhase(phase, tiles))
				return false;
	return true;
}

bool Entity::processPhase(xmlNode* phase, const TiledImage& tiles)
{
	/* Each phase requires a 'name'. Additionally,
	 * one of either 'pos' or 'speed' is needed.
	 * If speed is used, we have sub-elements. We
	 * can't have both pos and speed.
	 */
	const std::string name = (char*)xmlGetProp(phase, BAD_CAST("name"));

	const xmlChar* posStr = xmlGetProp(phase, BAD_CAST("pos"));
	const xmlChar* speedStr = xmlGetProp(phase, BAD_CAST("speed"));

	// FIXME: check name + pos | speed for 0 length

	if (posStr && speedStr) {
		Log::err(descriptor, "pos and speed attributes in "
				"element phase are mutually exclusive");
		return false;
	}
	if (!posStr && !speedStr) {
		Log::err(descriptor, "must have pos or speed attribute "
			       "in element phase");
		return false;
	}

	if (posStr) {
		// atoi
		const unsigned pos = (unsigned)atoi((const char*)posStr);
		// FIXME: check for out of bounds
		phases[name] = Animation(tiles[pos]);
	}
	else { // speedStr
		// atoi
		const double speed = (unsigned)atof((const char*)speedStr);
		// FIXME: check for out of bounds

		phases[name] = Animation();
		int len = (int)(1000.0/speed);
		phases[name].setFrameLen(len);
		for (xmlNode* member = phase->xmlChildrenNode; member != NULL;
				member = member->next)
			if (!xmlStrncmp(member->name, BAD_CAST("member"), 7)) // needed?
				if (!processMember(member, phases[name], tiles))
					return false;
	}

	return true;
}

bool Entity::processMember(xmlNode* phase, Animation& anim,
                           const TiledImage& tiles)
{
	const xmlChar* posStr = xmlGetProp(phase, BAD_CAST("pos"));
	const unsigned pos = (unsigned)atoi((const char*)posStr); // atoi
	// FIXME: check for out of bounds
	anim.addFrame(tiles[pos]);
	return true;
}

bool Entity::processSounds(const xmlNode* sounds)
{
	for (xmlNode* sound = sounds->xmlChildrenNode; sound != NULL;
			sound = sound->next)
		if (!xmlStrncmp(sound->name, BAD_CAST("sound"), 6)) // needed?
			if (!processSound(sound))
				return false;
	return true;
}

bool Entity::processSound(xmlNode* sound)
{
	const std::string name = (char*)xmlGetProp(sound, BAD_CAST("name"));
	const std::string filename = (char*)xmlNodeGetContent(sound);
	// FIXME: check name, filename for 0 length

	SampleRef s = rc->getSample(filename);
	if (s)
		sounds[name] = s;
	else
		Log::err(descriptor, std::string("sound ") +
				filename + " not found");
	return s;
}

void Entity::preMove(coord_t)
{
}

void Entity::postMove()
{
}

