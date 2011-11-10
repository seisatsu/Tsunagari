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
	  speedMul(1.0),
	  moving(false),
	  c(0, 0, 0),
	  r(0.0, 0.0, 0.0),
	  conf(conf)
{
	if (area)
		setArea(area);
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
	phase->frame()->draw(doff.x + r.x, doff.y + r.y, 0.0);
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
			(double)destCoord.x, (double)destCoord.y);
	if (destDist < traveled) {
		c = destCoord;
		moving = false;
		postMove();
	}
	else {
		double angle = angleFromXY(c.x - destCoord.x, destCoord.y - c.y);
		double dx = cos(angle);
		double dy = -sin(angle);

		// Fix inaccurate trig functions. (Why do I have to do this!??)
		if (-1e-10 < dx && dx < 1e-10)
			dx = 0.0;
		if (-1e-10 < dy && dy < 1e-10)
			dy = 0.0;

		// Save state of partial pixel travel in double.
		r.x += dx * traveled;
		r.y += dy * traveled;

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
	const icoord tileDim = area->getTileDimensions();
	c = coords;
	c *= tileDim;
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
	// FIXME: missing r =
	redraw = true;
}

void Entity::moveByTile(icoord delta)
{
	if (conf->moveMode == TILE && moving)
		// support queueing moves?
		return;

	icoord newCoord = getTileCoords();
	newCoord += delta;

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
	fromCoord = c;
	fromTile = &getTile();
	const icoord tileDim = area->getTileDimensions();
	destCoord = fromCoord;
	destCoord /= tileDim;
	destCoord += delta;
	destCoord *= tileDim;
	destTile = &area->getTile(newCoord);

	redraw = true;

	preMove(delta);

	if (conf->moveMode == TURN) {
		c = destCoord;
		postMove();
	}
	else if (conf->moveMode == TILE) {
		moving = true;
	}
}

void Entity::setArea(Area* a)
{
	area = a;
	calcDoff();
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

void Entity::calcDoff()
{
	// X-axis is centered with tile.
	doff.x = (area->getTileDimensions().x - imgw) / 2;
	// Y-axis is aligned with bottom of tile.
	doff.y = area->getTileDimensions().y - imgh;
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
}

void Entity::preMoveLua()
{
	const std::string& name = scripts["premove"];
	if (name.size()) {
		Script s(rc);
		bindEntity(&s, this, "entity");
		s.run(rc, name);
	}
}

void Entity::postMoveLua()
{
	const std::string& name = scripts["postmove"];
	if (name.size()) {
		Script s(rc);
		bindEntity(&s, this, "entity");
		s.run(rc, name);
	}
}

void Entity::postMove()
{
	if (conf->moveMode != TURN)
		setPhase(facing);
	fromTile->onLeaveScripts(rc, this);
	postMoveLua();
	destTile->onEnterScripts(rc, this);

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
	XMLDoc doc = rc->getXMLDoc(descriptor, "entity.dtd");
	if (!doc)
		return false;
	const XMLNode root = doc.root(); // <entity>
	if (!root)
		return false;

	for (XMLNode node = root.childrenNode(); node; node = node.next()) {
		if (node.is("speed")) {
			if (!node.doubleContent(&baseSpeed))
				return false;
			speed = baseSpeed /= 1000.0;
		} else if (node.is("sprite")) {
			if (!processSprite(node.childrenNode()))
				return false;
		} else if (node.is("sounds")) {
			if (!processSounds(node.childrenNode()))
				return false;
		} else if (node.is("scripts")) {
			if (!processScripts(node.childrenNode()))
				return false;
		}
	}
	return true;
}

bool Entity::processSprite(XMLNode node)
{
	for (; node; node = node.next()) {
		if (node.is("sheet")) {
			xml.sheet = node.content();
			if (!node.intAttr("tilewidth",  &imgw) ||
			    !node.intAttr("tileheight", &imgh))
				return false;
		} else if (node.is("phases")) {
			if (!processPhases(node.childrenNode()))
				return false;
		}
	}
	return true;
}

bool Entity::processPhases(XMLNode node)
{
	TiledImage tiles;
	if (!rc->getTiledImage(tiles, xml.sheet, (unsigned)imgw,
			(unsigned)imgh, false))
		return false;
	for (; node; node = node.next())
		if (node.is("phase"))
			if (!processPhase(node, tiles))
				return false;
	return true;
}

bool Entity::processPhase(const XMLNode node, const TiledImage& tiles)
{
	/* Each phase requires a 'name'. Additionally,
	 * one of either 'pos' or 'speed' is needed.
	 * If speed is used, we have sub-elements. We
	 * can't have both pos and speed.
	 */
	const std::string name = node.attr("name");
	if (name.empty()) {
		Log::err(descriptor, "<phase> name attribute is empty");
		return false;
	}

	const std::string posStr = node.attr("pos");
	const std::string speedStr = node.attr("speed");

	if (posStr.size() && speedStr.size()) {
		Log::err(descriptor, "pos and speed attributes in "
				"phase element are mutually exclusive");
		return false;
	} else if (posStr.empty() && speedStr.empty()) {
		Log::err(descriptor, "must have pos or speed attribute "
			       "in phase element");
		return false;
	}

	if (posStr.size()) {
		int pos;
		if (!node.intAttr("pos", &pos))
			return false;
		if (pos < 0 || (int)tiles.size() < pos) {
			Log::err(descriptor,
				"<phase></phase> index out of bounds");
			return false;
		}
		phases[name].addFrame(tiles[pos]);
	}
	else {
		int speed;
		if (!node.intAttr("speed", &speed))
			return false;

		int len = (int)(1000.0/speed);
		phases[name].setFrameLen(len);
		if (!processMembers(node.childrenNode(), phases[name], tiles))
			return false;
	}

	return true;
}

bool Entity::processMembers(XMLNode node, Animation& anim,
                            const TiledImage& tiles)
{
	for (; node; node = node.next())
		if (node.is("member"))
			if (!processMember(node, anim, tiles))
				return false;
	return true;
}

bool Entity::processMember(const XMLNode node, Animation& anim,
                           const TiledImage& tiles)
{
	int pos;
	if (!node.intAttr("pos", &pos))
		return false;
	if (pos < 0 || (int)tiles.size() < pos) {
		Log::err(descriptor, "<member></member> index out of bounds");
		return false;
	}
	anim.addFrame(tiles[pos]);
	return true;
}

bool Entity::processSounds(XMLNode node)
{
	for (; node; node = node.next())
		if (node.is("sound"))
			if (!processSound(node))
				return false;
	return true;
}

bool Entity::processSound(const XMLNode node)
{
	const std::string name = node.attr("name");
	const std::string filename = node.content();
	if (name.empty()) {
		Log::err(descriptor, "<sound> name attribute is empty");
		return false;
	} else if (filename.empty()) {
		Log::err(descriptor, "<sound></sound> is empty");
		return false;
	}

	SampleRef s = rc->getSample(filename);
	if (s)
		sounds[name] = s;
	return true;
}

bool Entity::processScripts(XMLNode node)
{
	for (; node; node = node.next())
		if (node.is("script"))
			if (!processScript(node))
				return false;
	return true;
}

bool Entity::processScript(const XMLNode node)
{
	const std::string trigger = node.attr("trigger");
	const std::string filename = node.content();
	if (trigger.empty()) {
		Log::err(descriptor, "<script> trigger attribute is empty");
		return false;
	} else if (filename.empty()) {
		Log::err(descriptor, "<script></script> is empty");
		return false;
	}

	// Don't verify for script exists here. This happens when it's
	// triggered.
	scripts[trigger] = filename;
	return true;
}

