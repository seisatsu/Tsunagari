/*********************************
** Tsunagari Tile Engine        **
** world.cpp                    **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <Gosu/Utility.hpp>

#include "area-tmx.h"
#include "log.h"
#include "python.h"
#include "resourcer.h"
#include "timeout.h"
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
	: music(new Music()), clips(0)
{
	globalWorld = this;
	pythonSetGlobal("World", this);
}

World::~World()
{
}

bool World::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return false;

	if (!player.init(playerentity))
		return false;
	player.setPhase("down");

	pythonSetGlobal("Player", (Entity*)&player);
	loadScript.invoke();

	view.reset(new Viewport(viewport));
	view->trackEntity(&player);

	Area* area = getArea(entry.area);
	if (!area)
		return false;
	focusArea(area, entry.coords);
	return true;
}

const std::string& World::getName()
{
	return name;
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

	pushLetterbox();
	graphics.pushTransform(getTransform());

	area->draw();

	graphics.popTransform();
	popLetterbox();
}

bool World::needsRedraw() const
{
	return area->needsRedraw();
}

void World::tick(unsigned long dt)
{
	updateTimeouts();
	area->tick(dt);
}

void World::turn()
{
	if (conf.moveMode == TURN) {
		updateTimeouts();
		area->turn();
	}
}

Area* World::getArea(const std::string& filename)
{
	AreaMap::iterator entry = areas.find(filename);
	if (entry != areas.end())
		return entry->second;

	Area* newArea = new AreaTMX(view.get(), &player, music.get(), filename);

	if (!newArea->init())
		newArea = NULL;
	areas[filename] = newArea;
	return newArea;
}

Area* World::getFocusedArea()
{
	return area;
}

void World::focusArea(Area* area, int x, int y, double z)
{
	focusArea(area, vicoord(x, y, z));
}

void World::focusArea(Area* area, vicoord playerPos)
{
	this->area = area;
	player.setArea(area);
	player.setTileCoords(playerPos);
	view->setArea(area);
	area->focus();
}

void World::runAreaLoadScript(Area* area)
{
	pythonSetGlobal("Area", area);
	areaLoadScript.invoke();
}

void World::pushLetterbox()
{
	GameWindow& w = GameWindow::instance();
	Gosu::Graphics& g = w.graphics();

	// Aspect ratio correction.
	rvec2 sz = view->getPhysRes();
	rvec2 lb = -1 * view->getLetterboxOffset();

	g.beginClipping(lb.x, lb.y, sz.x - 2 * lb.x, sz.y - 2 * lb.y);
	clips++;

	// Map bounds.
	rvec2 scale = view->getScale();
	rvec2 virtScroll = view->getMapOffset();
	rvec2 padding = view->getLetterboxOffset();

	rvec2 physScroll = -1 * virtScroll * scale + padding;

	bool loopX = area->loopsInX();
	bool loopY = area->loopsInY();

	if (!loopX && physScroll.x > 0) {
		// Boxes on left-right.
		g.beginClipping(physScroll.x, 0, sz.x - 2 * physScroll.x, sz.x);
		clips++;
	}
	if (!loopY && physScroll.y > 0) {
		// Boxes on top-bottom.
		g.beginClipping(0, physScroll.y, sz.x, sz.y - 2 * physScroll.y);
		clips++;
	}
}

void World::popLetterbox()
{
	GameWindow& w = GameWindow::instance();
	for (; clips; clips--)
		w.graphics().endClipping();
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

	if (conf.moveMode == TURN &&
	   (conf.persistInit == 0 || conf.persistCons == 0)) {
		Log::fatal("world.conf", "\"input->persist\" option required in \"turn\" mode");
		return false;
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
		std::string filename = node.content();
		ScriptInst script(filename);

		if (node.is("on_init")) {
			if (!script.validate("world.conf"))
				return false;
			loadScript = filename;
		} else if (node.is("on_area_init")) {
			if (!script.validate("world.conf"))
				return false;
			areaLoadScript = filename;
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

void exportWorld()
{
	using namespace boost::python;

	class_<World> ("World", no_init)
		.def("area", &World::getArea,
			return_value_policy<reference_existing_object>())
		.def("focus",
			static_cast<void (World::*) (Area*,int,int,double)>
			(&World::focusArea))
		;
}

