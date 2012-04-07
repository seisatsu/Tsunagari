/*********************************
** Tsunagari Tile Engine        **
** world.cpp                    **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <Gosu/Utility.hpp>

#include "area-tmx.h"
#include "common.h"
#include "log.h"
#include "python.h"
#include "resourcer.h"
#include "window.h"
#include "world.h"
#include "xml.h"

#define ASSERT(x)  if (!(x)) return false

static World* globalWorld = NULL;

World* World::instance()
{
	return globalWorld;
}

World::World()
	: player(NULL)
{
	globalWorld = this;
}

World::~World()
{
}

bool World::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return false;

	music.reset(new Music());

	if (!player.init(playerentity))
		return false;
	player.setPhase("down");

	if (onLoadScript.size()) {
		pythonSetGlobal("Player", (Entity*)&player);
		Resourcer* rc = Resourcer::instance();
		rc->runPythonScript(onLoadScript);
	}

	view = new Viewport(viewport);
	view->trackEntity(&player);

	AreaPtr area = getArea(entry.area);
	if (!area)
		return false;
	focusArea(area, entry.coords);
	return true;
}

void World::buttonDown(const Gosu::Button btn)
{
	area->buttonDown(btn);
}

void World::buttonUp(const Gosu::Button btn)
{
	area->buttonUp(btn);
}

void World::draw()
{
	GameWindow& window = GameWindow::instance();
	Gosu::Graphics& graphics = window.graphics();

	drawLetterbox();
	drawAreaBorders();
	graphics.pushTransform(getTransform());
	area->draw();
	graphics.popTransform();
}

bool World::needsRedraw() const
{
	return area->needsRedraw();
}

void World::update(unsigned long dt)
{
	// Prevent the Area from being deleted during this function call.
	// Otherwise if it accesses its member variables and we've overwritten
	// the memory, bad things happen.
	boost::shared_ptr<Area> safe(area);

	area->update(dt);
}

AreaPtr World::getArea(const std::string& filename, int flags)
{
	if (conf.cacheEnabled && (flags & GETAREA_ALWAYS_CREATE) == false) {
		AreaMap::iterator entry = areas.find(filename);
		if (entry != areas.end())
			return entry->second;
	}

	AreaPtr newArea(
		new AreaTMX(view, &player, music.get(), filename)
	);

	if (!newArea->init())
		newArea = AreaPtr();
	if (conf.cacheEnabled)
		areas[filename] = newArea;
	return newArea;
}

AreaPtr World::getFocusedArea()
{
	return area;
}

void World::focusArea(AreaPtr area, vicoord playerPos)
{
	// Log::info("World", area->getDescriptor() + ": focused");
	this->area = area;
	player.setArea(area.get());
	player.setTileCoords(playerPos);
	view->setArea(area.get());
	area->focus();
}

std::string World::getAreaLoadScript()
{
	return onAreaLoadScript;
}

bool World::processDescriptor()
{
	XMLRef doc;
	XMLNode root;

	Resourcer* rc = Resourcer::instance();
	ASSERT(doc = rc->getXMLDoc("world.conf", "world.dtd"));
	ASSERT(root = doc->root()); // <world>

	for (XMLNode child = root.childrenNode(); child; child = child.next()) {
		if (child.is("info")) {
			ASSERT(processInfo(child));
		}
		else if (child.is("init")) {
			ASSERT(processInit(child));
		}
		else if (child.is("script")) {
			ASSERT(processScript(child));
		}
		else if (child.is("input")) {
			ASSERT(processInput(child));
		}
	}

	return true;
}

bool World::processInfo(XMLNode node)
{
	for (node = node.childrenNode(); node; node = node.next()) {
		if (node.is("name")) {
			name = node.content();
			GameWindow::instance().setCaption(Gosu::widen(name));
		} else if (node.is("author")) {
			author = node.content();
		} else if (node.is("version")) {
			version = atof(node.content().c_str());
		}
	}
	return true;
}

bool World::processInit(XMLNode node)
{
	for (node = node.childrenNode(); node; node = node.next()) {
		if (node.is("area")) {
			entry.area = node.content();
		}
		else if (node.is("player")) {
			playerentity = node.content();
		}
		else if (node.is("mode")) {
			std::string str = node.content();
			if (str == "turn")
				conf.moveMode = TURN;
			else if (str == "tile")
				conf.moveMode = TILE;
			else if (str == "notile")
				conf.moveMode = NOTILE;
		}
		else if (node.is("coords")) {
			if (!node.intAttr("x", &entry.coords.x) ||
			    !node.intAttr("y", &entry.coords.y) ||
			    !node.doubleAttr("z", &entry.coords.z))
				return false;
		}
		else if (node.is("viewport")) {
			if (!node.intAttr("width", &viewport.x) ||
			    !node.intAttr("height", &viewport.y))
				return false;
		}
	}
	return true;
}

bool World::processScript(XMLNode node)
{
	for (node = node.childrenNode(); node; node = node.next()) {
		if (node.is("on_init")) {
			std::string filename = node.content();
			if (rc->resourceExists(filename)) {
				onLoadScript = filename;
			}
			else {
				Log::err("world.conf",
					std::string("script not found: ") + filename);
				return false;
			}
		} else if (node.is("on_area_init")) {
			std::string filename = node.content();
			if (rc->resourceExists(filename)) {
				onAreaLoadScript = filename;
			}
			else {
				Log::err("world.conf",
					std::string("script not found: ") + filename);
				return false;
			}
		}
	}
	return true;
}

bool World::processInput(XMLNode node)
{
	for (node = node.childrenNode(); node; node = node.next()) {
		if (node.is("persist")) {
			if (!node.intAttr("init", &conf.persistInit) ||
				!node.intAttr("cons", &conf.persistCons))
			    return false;
		}
	}
	return true;
}

void World::drawLetterbox()
{
	rvec2 sz = view->getPhysRes();
	rvec2 lb = rvec2(0.0, 0.0);
	lb -= view->getLetterboxOffset();
	Gosu::Color black = Gosu::Color::BLACK;

	drawRect(0, sz.x, 0, lb.y, black, 1000);
	drawRect(0, sz.x, sz.y - lb.y, sz.y, black, 1000);
	drawRect(0, lb.x, 0, sz.y, black, 1000);
	drawRect(sz.x - lb.x, sz.x, 0, sz.y, black, 1000);
}

void World::drawAreaBorders()
{
	Gosu::Color black = Gosu::Color::BLACK;
	rvec2 sz = view->getPhysRes();
	rvec2 scale = view->getScale();
	rvec2 virtScroll = view->getMapOffset();
	rvec2 padding = view->getLetterboxOffset();

	rvec2 physScroll = virtScroll;
	physScroll *= scale;
	physScroll += padding;
	physScroll *= -1;

	bool loopX = area->loopsInX();
	bool loopY = area->loopsInY();

	if (!loopX && physScroll.x > 0) {
		// Boxes on left-right.
		drawRect(0, physScroll.x, 0, sz.y, black, 500);
		drawRect(sz.x - physScroll.x, sz.x, 0, sz.y, black, 500);
	}
	if (!loopY && physScroll.y > 0) {
		// Boxes on top-bottom.
		drawRect(0, sz.x, 0, physScroll.y, black, 500);
		drawRect(0, sz.x, sz.y - physScroll.y, sz.y, black, 500);
	}
}

void World::drawRect(double x1, double x2, double y1, double y2,
                       Gosu::Color c, double z)
{
	GameWindow& window = GameWindow::instance();
	window.graphics().drawQuad(
		x1, y1, c,
		x2, y1, c,
		x2, y2, c,
		x1, y2, c,
		z
	);
}

Gosu::Transform World::getTransform()
{
	rvec2 scale = view->getScale();
	rvec2 scroll = view->getMapOffset();
	rvec2 padding = view->getLetterboxOffset();
	Gosu::Transform t = { {
		scale.x, 0,       0, 0,
		0,       scale.y, 0, 0,
		0,       0,       1, 0,
		scale.x * -scroll.x - padding.x,
		scale.y * -scroll.y - padding.y, 0, 1
	} };
	return t;
}

