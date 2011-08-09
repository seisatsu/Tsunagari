/******************************
** Tsunagari Tile Engine     **
** entity.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <math.h>

#include <Gosu/Image.hpp>
#include <Gosu/Math.hpp>
#include <Gosu/Timing.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "area.h"
#include "config.h"
#include "entity.h"
#include "entity-lua.h"
#include "log.h"
#include "resourcer.h"
#include "script.h"
#include "window.h"

static std::string facings[][3] = {
	{"up-left",   "up",   "up-right"},
	{"left",      "",     "right"},
	{"down-left", "down", "down-right"},
};


Entity::Entity(Resourcer* rc, Area* area, ClientValues* conf)
	: rc(rc),
	  redraw(true),
	  moving(false),
	  speed(240.0 / 1000), // FIXME
	  area(area),
	  conf(conf)
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
	phase = &phases.begin()->second;
	return true;
}

void Entity::draw()
{
	int millis = GameWindow::getWindow().time();
	phase->updateFrame(millis);
	phase->frame()->draw((double)c.x, (double)c.y, (double)0);
	redraw = false;
}

bool Entity::needsRedraw() const
{
	int millis = GameWindow::getWindow().time();
	return redraw || phase->needsRedraw(millis);
}


static double angleFromXY(long x, long y)
{
	double angle = 0.0;

	// Moving at an angle
	if (x != 0 && y != 0) {
		angle = atan((double)y / (double)x);
		if (y < 0 && x < 0)
			;
		else if (y < 0 && x > 0)
			angle += M_PI;
		else if (y > 0 && x < 0)
			angle += M_PI*2;
		else if (y > 0 && x > 0)
			angle += M_PI;
	}

	// Moving straight
	else {
		if (x < 0)
			angle = 0;
		else if (x > 0)
			angle = M_PI;
		else if (y < 0)
			angle = M_PI_2;
		else if (y > 0)
			angle = 3*M_PI_2;
	}

	return angle;
}

void Entity::update(unsigned long dt)
{
	if (conf->movemode == TILE && moving) {
		redraw = true;

		double destDist = Gosu::distance((double)c.x, (double)c.y,
				(double)dest.x, (double)dest.y); 
		if (destDist < speed * (double)dt) {
			c = dest;
			moving = false;
			postMove();
		}
		else {
			double angle = angleFromXY(c.x - dest.x, dest.y - c.y);
			double dx = cos(angle);
			double dy = -sin(angle);

			// Fix inaccurate trig functions
			if (-1e-10 < dx && dx < 1e-10)
				dx = 0.0;
			if (-1e-10 < dy && dy < 1e-10)
				dy = 0.0;

			// Save state of partial pixels traveled in double
			rx += dx * speed * (double)dt;
			ry += dy * speed * (double)dt;

			c.x = (long)rx;
			c.y = (long)ry;
		}
	}
}

bool Entity::setPhase(const std::string& name)
{
	boost::unordered_map<std::string, Animation>::iterator it;
	it = phases.find(name);
	if (it != phases.end()) {
		Animation* newPhase = &it->second;
		if (phase != newPhase) {
			phase = newPhase;
			redraw = true;
			return true;
		}
	}
	return false;
}

coord_t Entity::getCoordsByPixel() const
{
	return c;
}

coord_t Entity::getCoordsByTile() const
{
	coord_t tileDim = area->getTileDimensions();
	// XXX: revisit when we have Z-buffers
	return coord(c.x / tileDim.x, c.y / tileDim.y, c.z);
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

void Entity::moveByPixel(coord_t delta)
{
	c.x += delta.x;
	c.y += delta.y;
	c.z += delta.z;
	redraw = true;
}

void Entity::moveByTile(coord_t delta)
{
	if (conf->movemode == TILE && moving)
		// support queueing moves?
		return;

	coord_t newCoord = getCoordsByTile();
	newCoord.x += delta.x;
	newCoord.y += delta.y;
	newCoord.z += delta.z;

	// Can we move?
	const Area::Tile& tile = area->getTile(newCoord);
	if ((tile.flags       & Area::nowalk) != 0 ||
	    (tile.type->flags & Area::nowalk) != 0) {
		// The tile we're trying to move onto is set as nowalk.
		// Turn to face the direction, but don't move.
		calculateFacing(delta);
		setPhase(facing);
		return;
	}

	// Move!
	const coord_t tileDim = area->getTileDimensions();
	dest.x = c.x + delta.x * tileDim.x;
	dest.y = c.y + delta.y * tileDim.y;
	dest.z = 0; // XXX: set dest.z when we have Z-buffers
	redraw = true;

	preMove(delta);

	if (conf->movemode == TURN) {
		c.x = dest.x;
		c.y = dest.y;
		// XXX: set c.z when we have Z-buffers
		postMove();
	}
	else if (conf->movemode == TILE) {
		moving = true;
		rx = (double)c.x;
		ry = (double)c.y;
	}
}

void Entity::setArea(Area* a)
{
	area = a;
}

void Entity::gotoRandomTile()
{
	coord_t map = area->getDimensions();
	coord_t pos;
	Area::Tile* tile;
	do {
		pos = coord(rand() % map.x, rand() % map.y, 0);
		tile = &area->getTile(pos);
	} while (((tile->flags & Area::nowalk) |
	          (tile->type->flags & Area::nowalk)) != 0);
	setCoordsByTile(pos);
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

void Entity::calculateFacing(coord_t delta)
{
	int x, y;

	if (delta.x < 0)
		x = 0;
	else if (delta.x == 0)
		x = 1;
	else
		x = 2;

	if (delta.y < 0)
		y = 0;
	else if (delta.y == 0)
		y = 1;
	else
		y = 2;

	facing = facings[y][x];
}

void Entity::preMove(coord_t delta)
{
	calculateFacing(delta);
	if (conf->movemode == TURN)
		setPhase(facing);
	else
		setPhase("moving " + facing);
}

void Entity::postMove()
{
	if (conf->movemode != TURN)
		setPhase(facing);
	postMoveHook();
}

void Entity::postMoveHook()
{
	if (rc->resourceExists("postMove.lua")) {
		const coord_t tile = getCoordsByTile();
		Script script;
		script.bindEntity("entity", this);
		script.bindObjFn("entity", "gotoRandomTile", lua_Entity_gotoRandomTile);
		script.bindInt("x", tile.x);
		script.bindInt("y", tile.y);
		script.run(rc, "postMove.lua");
	}
}

/**
 * Try to load in descriptor.
 */
bool Entity::processDescriptor()
{
	XMLDocRef doc = rc->getXMLDoc(descriptor, "entity.dtd");
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
	for (xmlNode* child = sprite->xmlChildrenNode; child != NULL;
			child = child->next) {
		if (!xmlStrncmp(child->name, BAD_CAST("sheet"), 6)) {
			xml.sheet = readXmlElement(child);

			xml.tileSize.x = atol(readXmlAttribute(child, "tilewidth").c_str());

			xml.tileSize.y = atol(readXmlAttribute(child, "tileheight").c_str());
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
	const std::string name = readXmlAttribute(phase, "name");
	
	const std::string posStr = readXmlAttribute(phase, "pos");
	const std::string speedStr = readXmlAttribute(phase, "speed");

	// FIXME: check name + pos | speed for 0 length

	if (!posStr.empty() && !speedStr.empty()) {
		Log::err(descriptor, "pos and speed attributes in "
				"element phase are mutually exclusive");
		return false;
	}
	if (posStr.empty() && speedStr.empty()) {
		Log::err(descriptor, "must have pos or speed attribute "
			       "in element phase");
		return false;
	}

	if (!posStr.empty()) {
		// atoi
		const unsigned pos = (unsigned)atoi(posStr.c_str());
		// FIXME: check for out of bounds
		phases[name] = Animation(tiles[pos]);
	}
	else { // speedStr
		// atoi
		const double speed = (unsigned)atof(speedStr.c_str());
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
	const std::string posStr = readXmlAttribute(phase, "pos");
	const unsigned pos = (unsigned)atoi(posStr.c_str()); // atoi
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
	const std::string name = readXmlAttribute(sound, "name");
	const std::string filename = readXmlElement(sound);
	// FIXME: check name, filename for 0 length

	SampleRef s = rc->getSample(filename);
	if (s)
		sounds[name] = s;
	else
		Log::err(descriptor, std::string("sound ") +
				filename + " not found");
	return s;
}

