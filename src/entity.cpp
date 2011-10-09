/******************************
** Tsunagari Tile Engine     **
** entity.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <math.h>

#include <boost/foreach.hpp>
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
#include "xml.h"

static std::string facings[][3] = {
	{"up-left",   "up",   "up-right"},
	{"left",      "",     "right"},
	{"down-left", "down", "down-right"},
};


Entity::Entity(Resourcer* rc, Area* area, ClientValues* conf)
	: rc(rc),
	  redraw(true),
	  moving(false),
	  area(area),
	  conf(conf)
{
	c = icoord(0, 0, 0);
	r = rcoord(0.0, 0.0, 0.0);
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
	phase->frame()->draw(r.x, r.y, 0.0);
	redraw = false;
}

bool Entity::needsRedraw() const
{
	int millis = GameWindow::getWindow().time();
	return redraw || phase->needsRedraw(millis);
}


static double angleFromXY(int x, int y)
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
	switch (conf->moveMode) {
	case TURN:
		updateTurn(dt);
		break;
	case TILE:
		updateTile(dt);
		break;
	case NOTILE:
		updateNoTile(dt);
		break;
	}
}

void Entity::updateTurn(unsigned long)
{
	// Entities don't do anything in TILE mode.
}

void Entity::updateTile(unsigned long dt)
{
	if (!moving)
		return;

	redraw = true;
	double traveled = speed * (double)dt;
	double destDist = Gosu::distance((double)c.x, (double)c.y,
			(double)dest.x, (double)dest.y); 
	if (destDist < traveled) {
		c = dest;
		moving = false;
		postMove();
	}
	else {
		double angle = angleFromXY(c.x - dest.x, dest.y - c.y);
		double dx = cos(angle);
		double dy = -sin(angle);

		// Fix inaccurate trig functions. (Why do I have to do this!??)
		if (-1e-10 < dx && dx < 1e-10)
			dx = 0.0;
		if (-1e-10 < dy && dy < 1e-10)
			dy = 0.0;

		// Save state of partial pixel travel in double.
		r.x += dx * speed * (double)dt;
		r.y += dy * speed * (double)dt;

		c.x = (int)r.x;
		c.y = (int)r.y;
	}
}

void Entity::updateNoTile(unsigned long)
{
	// TODO
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

icoord Entity::getIPixel() const
{
	return c;
}

rcoord Entity::getRPixel() const
{
	return r;
}

icoord Entity::getTileCoords() const
{
	icoord tileDim = area->getTileDimensions();
	// XXX: revisit when we have Z-buffers
	return icoord(c.x / tileDim.x, c.y / tileDim.y, c.z);
}

void Entity::setPixelCoords(icoord coords)
{
	redraw = true;
	c = coords;
	r.x = c.x;
	r.y = c.y;
	r.z = c.z;
}

void Entity::setTileCoords(icoord coords)
{
	redraw = true;
	icoord tileDim = area->getTileDimensions();
	c = coords;
	c.x *= tileDim.x;
	c.y *= tileDim.y;
	// XXX: set c.z when we have Z-buffers
	r.x = c.x;
	r.y = c.y;
	// r.z = c.z;
}

void Entity::moveByPixel(icoord delta)
{
	c.x += delta.x;
	c.y += delta.y;
	c.z += delta.z;
	redraw = true;
}

void Entity::moveByTile(icoord delta)
{
	if (conf->moveMode == TILE && moving)
		// support queueing moves?
		return;

	icoord newCoord = getTileCoords();
	newCoord.x += delta.x;
	newCoord.y += delta.y;
	newCoord.z += delta.z;

	// Can we move?
	const Tile& tile = area->getTile(newCoord);
	if ((tile.flags       & nowalk) != 0 ||
	    (tile.type->flags & nowalk) != 0) {
		// The tile we're trc.ying to move onto is set as nowalk.
		// Turn to face the direction, but don't move.
		calculateFacing(delta);
		setPhase(facing);
		return;
	}

	// Move!
	const icoord tileDim = area->getTileDimensions();
	dest.x = c.x + delta.x * tileDim.x;
	dest.y = c.y + delta.y * tileDim.y;
	dest.z = 0; // XXX: set dest.z when we have Z-buffers
	redraw = true;

	preMove(delta);

	if (conf->moveMode == TURN) {
		c.x = dest.x;
		c.y = dest.y;
		// XXX: set c.z when we have Z-buffers
		postMove();
	}
	else if (conf->moveMode == TILE) {
		moving = true;
		r.x = (double)c.x;
		r.y = (double)c.y;
	}
}

void Entity::setArea(Area* a)
{
	area = a;
}

void Entity::gotoRandomTile()
{
	icoord map = area->getDimensions();
	icoord pos;
	Tile* tile;
	do {
		pos = icoord(rand() % map.x, rand() % map.y, 0);
		tile = &area->getTile(pos);
	} while (((tile->flags & nowalk) |
	          (tile->type->flags & nowalk)) != 0);
	setTileCoords(pos);
}

void Entity::setSpeed(double multiplier)
{
	speedMul = multiplier;
	speed = baseSpeed * speedMul;
}

Tile& Entity::getTile()
{
	return area->getTile(getTileCoords());
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

void Entity::calculateFacing(icoord delta)
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

void Entity::preMove(icoord delta)
{
	calculateFacing(delta);
	if (conf->moveMode == TURN)
		setPhase(facing);
	else
		setPhase("moving " + facing);
	preMoveLua();
	movingFrom = &getTile();
}

void Entity::preMoveLua()
{
	const std::string& name = scripts["premove"];
	Script s(rc);
	bindEntity(&s, this, "entity");
	s.run(rc, name);
}

void Entity::postMoveLua()
{
	const std::string& name = scripts["postmove"];
	Script s(rc);
	bindEntity(&s, this, "entity");
	s.run(rc, name);
}

void Entity::postMove()
{
	if (conf->moveMode != TURN)
		setPhase(facing);
	movingFrom->onLeaveScripts(rc, this);
	postMoveLua();
	getTile().onEnterScripts(rc, this);

	// TODO: move teleportation here
	/*
	 * if (onDoor()) {
	 * 	leaveTile();
	 * 	moveArea(getDoor());
	 * 	postMoveLua();
	 * 	enterTile();
	 * }
	 */
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
		if (!xmlStrncmp(node->name, BAD_CAST("speed"), 6)) {
			speed = baseSpeed =
				(double)atol(readXmlElement(node).c_str())
				/ 1000.0;
			speedMul = 1.0;
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("sprite"), 7)) {
			if (!processSprite(node))
				return false;
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("sounds"), 7)) {
			if (!processSounds(node))
				return false;
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("scripts"), 8)) {
			processScripts(node);
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
			xml.tileSize.x = atoi(readXmlAttribute(child, "tilewidth").c_str());
			xml.tileSize.y = atoi(readXmlAttribute(child, "tileheight").c_str());
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
		phases[name].addFrame(tiles[pos]);
	}
	else { // speedStr
		// atoi
		const double speed = (unsigned)atof(speedStr.c_str());
		// FIXME: check for out of bounds

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

void Entity::processScripts(const xmlNode* scripts)
{
	for (xmlNode* script = scripts->xmlChildrenNode; script != NULL;
			script = script->next)
		if (!xmlStrncmp(script->name, BAD_CAST("script"), 7)) // needed?
			processScript(script);
}

void Entity::processScript(xmlNode* script)
{
	const std::string trigger = readXmlAttribute(script, "trigger");
	const std::string filename = readXmlElement(script);
	// FIXME: check name, filename for 0 length

	// Don't verify for script exists here. This happens when it's
	// triggered.
	scripts[trigger] = filename;
}

