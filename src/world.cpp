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
		pythonSetGlobal("player", (Entity*)&player);
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
	GameWindow& window = GameWindow::getWindow();
	Gosu::Graphics& graphics = window.graphics();

	drawLetterbox();
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
	Log::info("World", area->getDescriptor() + ": focused");
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
	Resourcer* rc = Resourcer::instance();
	XMLRef doc = rc->getXMLDoc("world.conf", "world.dtd");
	if (!doc)
		return false;
	const XMLNode root = doc->root(); // <world>
	if (!root)
		return false;

	for (XMLNode node = root.childrenNode(); node; node = node.next()) {
		if (node.is("name")) {
			name = node.content();
			GameWindow::getWindow().setCaption(Gosu::widen(name));
		} else if (node.is("author")) {
			author = node.content();
		} else if (node.is("player")) {
			playerentity = node.content();
		} else if (node.is("type")) {
			std::string str = node.attr("locality");
			if (str == "local")
				locality = LOCAL;
			else if (str == "network")
				locality = NETWORK;

			str = node.attr("movement");
			if (str == "turn")
				conf.moveMode = TURN;
			else if (str == "tile")
				conf.moveMode = TILE;
			else if (str == "notile")
				conf.moveMode = NOTILE;
		} else if (node.is("entrypoint")) {
			entry.area = node.attr("area");
			if (!node.intAttr("x", &entry.coords.x) ||
			    !node.intAttr("y", &entry.coords.y) ||
			    !node.doubleAttr("z", &entry.coords.z))
				return false;
		} else if (node.is("viewport")) {
			if (!node.intAttr("width", &viewport.x) ||
			    !node.intAttr("height", &viewport.y))
				return false;
		} else if (node.is("onLoad")) {
			std::string filename = node.content();
			if (rc->resourceExists(filename)) {
				onLoadScript = filename;
			}
			else {
				Log::err("world.conf",
				  std::string("script not found: ") + filename);
				return false;
			}
		} else if (node.is("onAreaLoad")) {
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

void World::drawRect(double x1, double x2, double y1, double y2,
                       Gosu::Color c, double z)
{
	GameWindow& window = GameWindow::getWindow();
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

