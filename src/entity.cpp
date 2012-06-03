/*********************************
** Tsunagari Tile Engine        **
** entity.cpp                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <math.h>

#include <boost/algorithm/string.hpp> // for iequals
#include <boost/foreach.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/Math.hpp>
#include <Gosu/Timing.hpp>

#include "area.h"
#include "config.h"
#include "entity.h"
#include "log.h"
#include "python.h"
#include "resourcer.h"
#include "window.h"
#include "xml.h"

#define ASSERT(x)  if (!(x)) return false

static std::string directions[][3] = {
	{"up-left",   "up",   "up-right"},
	{"left",      "",     "right"},
	{"down-left", "down", "down-right"},
};


Entity::Entity()
	: redraw(true),
	  area(NULL),
	  r(0.0, 0.0, 0.0),
	  frozen(false),
	  speedMul(1.0),
	  moving(false),
	  stillMoving(false),
	  nowalkFlags(TILE_NOWALK | TILE_NOWALK_NPC),
	  nowalkExempt(0),
	  phase(NULL),
	  phaseName("")
{
}

Entity::~Entity()
{
	pythonSetGlobal("Area", area);
	pythonSetGlobal("Entity", this);
	deleteScript.invoke();
}

bool Entity::init(const std::string& descriptor)
{
	this->descriptor = descriptor;
	return processDescriptor();
}

void Entity::draw()
{
	int millis = GameWindow::instance().time();
	phase->updateFrame(millis);
	phase->frame()->draw(doff.x + r.x, doff.y + r.y, r.z);
	redraw = false;
}

bool Entity::needsRedraw() const
{
	int millis = GameWindow::instance().time();
	return redraw || phase->needsRedraw(millis);
}


static double angleFromXY(double x, double y)
{
	double angle = 0.0;

	// Moving at an angle
	if (x != 0 && y != 0) {
		angle = atan(y / x);
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
	runUpdateScript();
	switch (conf.moveMode) {
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
	double destDist = Gosu::distance(r.x, r.y, destCoord.x, destCoord.y);
	if (destDist <= traveled) {
		r = destCoord;
		moving = false;
		postMove();
		if (moving) {
			// Time rollover.
			double perc = 1.0 - destDist/traveled;
			unsigned long remt = (unsigned long)(perc * (double)dt);
			update(remt);
		}
	}
	else {
		double angle = angleFromXY(r.x - destCoord.x,
		                           destCoord.y - r.y);
		double dx = cos(angle);
		double dy = -sin(angle);

		// Fix inaccurate trig functions. (Yay finite precision.)
		if (-1e-10 < dx && dx < 1e-10)
			dx = 0.0;
		if (-1e-10 < dy && dy < 1e-10)
			dy = 0.0;

		r.x += dx * traveled;
		r.y += dy * traveled;
	}
}

void Entity::updateNoTile(unsigned long)
{
	// TODO
}

const std::string Entity::getFacing() const
{
	return directionStr(facing);
}

bool Entity::setPhase(const std::string& name)
{
	AnimationMap::iterator it;
	it = phases.find(name);
	if (it == phases.end()) {
		Log::err(descriptor, "phase '" + name + "' not found");
		return false;
	}
	Animation* newPhase = &it->second;
	if (phase != newPhase) {
		int now = GameWindow::instance().time();
		phase = newPhase;
		phase->startOver(now);
		phaseName = name;
		redraw = true;
		return true;
	}
	return false;
}

std::string Entity::getPhase() const
{
	return phaseName;
}

rcoord Entity::getPixelCoord() const
{
	return r;
}

icoord Entity::getTileCoords_i() const
{
	return area->virt2phys(r);
}

vicoord Entity::getTileCoords_vi() const
{
	return area->virt2virt(r);
}

void Entity::setTileCoords(int x, int y)
{
	leaveTile();
	vicoord virt(x, y, r.z);
	if (!area->inBounds(virt))
		return;
	redraw = true;
	r = area->virt2virt(virt);
	enterTile();
}

void Entity::setTileCoords(int x, int y, double z)
{
	leaveTile();
	vicoord virt(x, y, z);
	if (!area->inBounds(virt))
		return;
	redraw = true;
	r = area->virt2virt(virt);
	enterTile();
}

void Entity::setTileCoords(icoord phys)
{
	leaveTile();
	if (!area->inBounds(phys))
		return;
	redraw = true;
	r = area->phys2virt_r(phys);
	enterTile();
}

void Entity::setTileCoords(vicoord virt)
{
	leaveTile();
	if (!area->inBounds(virt))
		return;
	redraw = true;
	r = area->virt2virt(virt);
	enterTile();
}

bool Entity::isMoving() const
{
	return moving || stillMoving;
}

void Entity::moveByTile(int x, int y)
{
	moveByTile(ivec2(x, y));
}

void Entity::moveByTile(ivec2 delta)
{
	if (moving)
		return;
	setFacing(delta);

	std::vector<icoord> tiles = frontTiles();
	BOOST_FOREACH(const icoord& tile, tiles) {
		if (canMove(tile)) {
			preMove();
			return;
		}
		else
			setPhase(directionStr(facing));
	}
}

Area* Entity::getArea()
{
	return area;
}

void Entity::setArea(Area* a)
{
	leaveTile();
	area = a;
	calcDoff();
	setSpeed(speedMul); // Calculate new speed based on tile size.
	enterTile();
}

double Entity::getSpeed() const
{
	return speedMul;
}

void Entity::setSpeed(double multiplier)
{
	speedMul = multiplier;
	if (area) {
		double tilesPerSecond = area->getTileDimensions().x / 1000.0;
		speed = baseSpeed * speedMul * tilesPerSecond;
	}
}

Tile* Entity::getTile() const
{
	return area ? area->getTile(r) : NULL;
}

Tile* Entity::getTile()
{
	return area ? area->getTile(r) : NULL;
}

void Entity::setFrozen(bool b)
{
	frozen = b;
}

bool Entity::getFrozen()
{
	return frozen;
}

FlagManip Entity::exemptManip()
{
	return FlagManip(&nowalkExempt);
}

std::vector<icoord> Entity::frontTiles() const
{
	std::vector<icoord> tiles;
	icoord dest = getTileCoords_i() + icoord(facing.x, facing.y, 0);

	boost::optional<double> layermod = getTile()->layermodAt(facing);
	if (layermod)
		dest = area->virt2phys(vicoord(dest.x, dest.y, *layermod));
	tiles.push_back(dest);
	return tiles;
}

void Entity::calcDoff()
{
	// X-axis is centered on tile.
	doff.x = (area->getTileDimensions().x - imgw) / 2;
	// Y-axis is aligned with bottom of tile.
	doff.y = area->getTileDimensions().y - imgh - 1;
}

SampleRef Entity::getSound(const std::string& name) const
{
	SampleMap::const_iterator it;
	it = sounds.find(name);
	if (it != sounds.end())
		return it->second;
	else
		return SampleRef();
}

ivec2 Entity::setFacing(ivec2 facing)
{
	this->facing = ivec2(
		Gosu::clamp(facing.x, -1, 1),
		Gosu::clamp(facing.y, -1, 1)
	);
	return this->facing;
}

const std::string& Entity::directionStr(ivec2 facing) const
{
	return directions[facing.y+1][facing.x+1];
}

bool Entity::canMove(icoord dest)
{
	icoord delta = dest - getTileCoords_i();
	ivec2 dxy(delta.x, delta.y);
	bool inBounds = area->inBounds(dest);

	destCoord = area->phys2virt_r(dest);
	destTile = NULL;

	if (inBounds) {
		// Tile is inside map. Can we move?
		destTile = area->getTile(dest);
		if (nowalked(*destTile))
			return false;
		if (destTile->entCnt) // Space is occupied by another Entity.
			return false;
		return true;
	}
	else if (delta.z == 0 && getTile()->exitAt(dxy)) {
		// Even if it's out of map bounds, we could be coming from a
		// directional Exit that just points off the map.
	       if (nowalkExempt & TILE_NOWALK_AREA_BOUND)
			return true;
	}

	// The tile is legitimately off the map.
	return false;
}

bool Entity::nowalked(Tile& t)
{
	unsigned flags = nowalkFlags & ~nowalkExempt;
	return t.hasFlag(flags);
}

void Entity::preMove()
{
	fromCoord = r;
	fromTile = getTile();

	rcoord d = destCoord;
	d -= fromCoord;
	deltaCoord = area->virt2virt(d);

	moving = true;

	// Start moving animation.
	switch (conf.moveMode) {
	case TURN:
		break;
	case TILE:
	case NOTILE:
		setPhase("moving " + getFacing());
		break;
	}

	// Process triggers.
	runTileExitScript();
	fromTile->runLeaveScript(this);

	// Modify tile's entity count.
	leaveTile();
	enterTile(destTile);

	SampleRef step = getSound("step");
	if (step)
		step->play();

	// Set z right away so that we're on-level with the square we're
	// entering.
	r.z = destCoord.z;

	if (conf.moveMode == TURN) {
		// Movement is instantaneous.
		redraw = true;
		r = destCoord;
		postMove();
	}
	else {
		// Movement happens over time. See updateTile().
	}
}

void Entity::postMove()
{
	moving = false;

	if (destTile) {
		boost::optional<double> layermod = getTile()->layermods[EXIT_NORMAL];
		if (layermod)
			r.z = *layermod;
	}

	// Stop moving animation.
	if (!stillMoving)
		setPhase(getFacing());

	// Process triggers.
	if (destTile) {
		destTile->runEnterScript(this);
		runTileEntryScript();
	}

	// TODO: move teleportation here
	/*
	 * if (onExit()) {
	 * 	leaveTile();
	 * 	moveArea(getExit());
	 * 	postMoveScript();
	 * 	enterTile();
	 * }
	 */
}

void Entity::leaveTile()
{
	if (area) {
		Tile* t = getTile();
		if (t)
			t->entCnt--;
	}
}

void Entity::enterTile()
{
	if (area)
		enterTile(getTile());
}

void Entity::enterTile(Tile* t)
{
	if (t)
		t->entCnt++;
}

void Entity::runUpdateScript()
{
	pythonSetGlobal("Area", area);
	pythonSetGlobal("Entity", this);
	pythonSetGlobal("Tile", getTile());
	updateScript.invoke();
}

void Entity::runTileExitScript()
{
	pythonSetGlobal("Area", area);
	pythonSetGlobal("Entity", this);
	pythonSetGlobal("Tile", getTile());
	tileExitScript.invoke();
}

void Entity::runTileEntryScript()
{
	pythonSetGlobal("Area", area);
	pythonSetGlobal("Entity", this);
	pythonSetGlobal("Tile", getTile());
	tileEntryScript.invoke();
}


/*
 * DESCRIPTOR CODE BELOW
 */

bool Entity::processDescriptor()
{
	Resourcer* rc = Resourcer::instance();
	XMLRef doc = rc->getXMLDoc(descriptor, "entity.dtd");
	if (!doc)
		return false;
	const XMLNode root = doc->root(); // <entity>
	if (!root)
		return false;

	for (XMLNode node = root.childrenNode(); node; node = node.next()) {
		if (node.is("speed")) {
			ASSERT(node.doubleContent(&baseSpeed));
			setSpeed(speedMul); // Calculate speed from tile size.
		} else if (node.is("sprite")) {
			ASSERT(processSprite(node.childrenNode()));
		} else if (node.is("sounds")) {
			ASSERT(processSounds(node.childrenNode()));
		} else if (node.is("scripts")) {
			ASSERT(processScripts(node.childrenNode()));
		}
	}
	return true;
}

bool Entity::processSprite(XMLNode node)
{
	Resourcer* rc = Resourcer::instance();
	TiledImage tiles;
	for (; node; node = node.next()) {
		if (node.is("sheet")) {
			std::string imageSheet = node.content();
			ASSERT(node.intAttr("tile_width",  &imgw) &&
			       node.intAttr("tile_height", &imgh));
			ASSERT(rc->getTiledImage(tiles, imageSheet,
			       imgw, imgh, false));
		} else if (node.is("phases")) {
			ASSERT(processPhases(node.childrenNode(), tiles));
		}
	}
	return true;
}

bool Entity::processPhases(XMLNode node, const TiledImage& tiles)
{
	for (; node; node = node.next())
		if (node.is("phase"))
			ASSERT(processPhase(node, tiles));
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
		ASSERT(node.intAttr("pos", &pos));
		if (pos < 0 || (int)tiles.size() < pos) {
			Log::err(descriptor,
				"<phase></phase> index out of bounds");
			return false;
		}
		phases[name].addFrame(tiles[pos]);
	}
	else {
		int speed;
		ASSERT(node.intAttr("speed", &speed));

		int len = (int)(1000.0/speed);
		phases[name].setFrameLen(len);
		ASSERT(processMembers(node.childrenNode(),
		                      phases[name], tiles));
	}

	return true;
}

bool Entity::processMembers(XMLNode node, Animation& anim,
                            const TiledImage& tiles)
{
	for (; node; node = node.next())
		if (node.is("member"))
			ASSERT(processMember(node, anim, tiles));
	return true;
}

bool Entity::processMember(const XMLNode node, Animation& anim,
                           const TiledImage& tiles)
{
	int pos;
	ASSERT(node.intAttr("pos", &pos));
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
			ASSERT(processSound(node));
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

	Resourcer* rc = Resourcer::instance();
	SampleRef s = rc->getSample(filename);
	if (s)
		sounds[name] = s;
	return true;
}

bool Entity::processScripts(XMLNode node)
{
	for (; node; node = node.next())
		if (node.is("script"))
			ASSERT(processScript(node));
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

	ScriptInst script(filename);
	if (!script.validate(descriptor))
		return false;

	if (!setScript(trigger, script)) {
		Log::err(descriptor,
			"unrecognized script trigger: " + trigger);
		return false;
	}

	return true;
}

bool Entity::setScript(const std::string& trigger, ScriptInst& script)
{
	if (boost::iequals(trigger, "on_update")) {
		updateScript = script;
		return true;
	}
	if (boost::equals(trigger, "on_tile_entry")) {
		tileEntryScript = script;
		return true;
	}
	if (boost::iequals(trigger, "on_tile_exit")) {
		tileExitScript = script;
		return true;
	}
	if (boost::iequals(trigger, "on_delete")) {
		deleteScript = script;
		return true;
	}
	return false;
}


void exportEntity()
{
	using namespace boost::python;

	class_<Entity>("Entity")
		.def("init", &Entity::init)
		.add_property("frozen", &Entity::getFrozen, &Entity::setFrozen)
		.add_property("phase", &Entity::getPhase, &Entity::setPhase)
		.add_property("area",
		    make_function(&Entity::getArea,
		      return_value_policy<reference_existing_object>()),
		    &Entity::setArea)
		.add_property("tile", make_function(
		    static_cast<Tile* (Entity::*) ()> (&Entity::getTile),
		    return_value_policy<reference_existing_object>()))
		.add_property("coords", &Entity::getTileCoords_vi)
		.add_property("speed", &Entity::getSpeed, &Entity::setSpeed)
		.add_property("moving", &Entity::isMoving)
		.add_property("exempt", &Entity::exemptManip)
		.def("set_coords",
		    static_cast<void (Entity::*) (int,int,double)>
		      (&Entity::setTileCoords))
		.def("move", static_cast<void (Entity::*) (int,int)>
		    (&Entity::moveByTile))
		.def("teleport", static_cast<void (Entity::*) (int,int)>
		    (&Entity::setTileCoords))
		.def("move",
		    static_cast<void (Entity::*) (int,int)>
		      (&Entity::moveByTile))
		.def_readwrite("on_update", &Entity::updateScript)
		.def_readwrite("on_tile_entry", &Entity::tileEntryScript)
		.def_readwrite("on_tile_exit", &Entity::tileExitScript)
		.def_readwrite("on_delete", &Entity::deleteScript)
		;
}

