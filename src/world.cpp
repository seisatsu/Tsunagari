/*********************************
** Tsunagari Tile Engine        **
** world.cpp                    **
** Copyright 2011-2012 OmegaSDG **
*********************************/

// "OmegaSDG" is defined as Michael D. Reiley and Paul Merrill.

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to 
// deal in the Software without restriction, including without limitation the 
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
// IN THE SOFTWARE.
// **********

#include <Gosu/Image.hpp>
#include <Gosu/Utility.hpp>

#include "area-tmx.h"
#include "log.h"
#include "music.h"
#include "python.h"
#include "timeout.h"
#include "window.h"
#include "world.h"
#include "xml.h"

#define ASSERT(x)  if (!(x)) { return false; }

static World* globalWorld = NULL;

World* World::instance()
{
	return globalWorld;
}

World::World()
	: total(0), redraw(false), userPaused(false), paused(0)
{
	globalWorld = this;
	lastTime = GameWindow::instance().time();
	pythonSetGlobal("World", this);
}

World::~World()
{
}

bool World::init()
{
	ASSERT(processDescriptor()); // Try to load in descriptor.
	ASSERT(pauseInfo = Reader::getImage("resource/pause_overlay.png"));

	ASSERT(player.init(playerEntity, playerPhase));
	pythonSetGlobal("Player", (Entity*)&player);

	view.reset(new Viewport(viewportSz));
	view->trackEntity(&player);

	loadScript.invoke();

	Area* area = getArea(startArea);
	if (area == NULL) {
		Log::fatal("World", "failed to load initial Area");
		return false;
	}
	focusArea(area, startCoords);

	return true;
}

const std::string& World::getName() const
{
	return name;
}

time_t World::time() const
{
	return total;
}

void World::buttonDown(const Gosu::Button btn)
{
	switch (btn.id()) {
	case Gosu::kbEscape:
		userPaused = !userPaused;
		setPaused(userPaused);
		redraw = true;
		break;
	default:
		if (!paused && keyStates.empty()) {
			area->buttonDown(btn);
			keydownScript.invoke();
		}
		break;
	}
}

void World::buttonUp(const Gosu::Button btn)
{
	switch (btn.id()) {
	case Gosu::kbEscape:
		break;
	default:
		if (!paused && keyStates.empty()) {
			area->buttonUp(btn);
			keyupScript.invoke();
		}
		break;
	}
}

void World::draw()
{
	redraw = false;

	GameWindow& window = GameWindow::instance();
	Gosu::Graphics& graphics = window.graphics();

	int clips = pushLetterbox();
	graphics.pushTransform(getTransform());

	area->draw();

	graphics.popTransform();
	popLetterbox(clips);

	if (paused) {
		unsigned ww = graphics.width();
		unsigned wh = graphics.height();
		Gosu::Color darken(127, 0, 0, 0);
		double top = std::numeric_limits<double>::max();

		drawRect(0, ww, 0, wh, darken, top);

		if (userPaused) {
			unsigned iw = pauseInfo->width();
			unsigned ih = pauseInfo->height();

			pauseInfo->draw(ww/2 - iw/2, wh/2 - ih/2, top);
		}
	}


}

bool World::needsRedraw() const
{
	if (redraw)
		return true;
	if (!paused && area->needsRedraw())
		return true;
	return false;
}

void World::update(time_t now)
{
	time_t dt = calculateDt(now);
	if (!paused) {
		total += dt;
		tick(dt);
	}
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

	Area* newArea = new AreaTMX(view.get(), &player, filename);

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

void World::setPaused(bool b)
{
	if (!paused && !b) {
		Log::err("World", "trying to unpause, but not paused");
		return;
	}

	// If just pausing.
	if (!paused)
		storeKeys();

	paused += b ? 1 : -1;

	if (paused)
		Music::setPaused(true);
	else
		Music::setPaused(false);

	// If finally unpausing.
	if (!paused)
		restoreKeys();
}

void World::storeKeys()
{
	keyStates.push(BitRecord::fromGosuInput());
}

void World::restoreKeys()
{
	BitRecord now = BitRecord::fromGosuInput();
	BitRecord then = keyStates.top();
	std::vector<size_t> diffs = now.diff(then);

	keyStates.pop();

	BOOST_FOREACH(size_t id, diffs) {
		Gosu::Button btn((unsigned)id);
		if (now[id])
			buttonDown(btn);
		else
			buttonUp(btn);
	}
}

void World::runAreaLoadScript(Area* area)
{
	pythonSetGlobal("Area", area);
	areaLoadScript.invoke();
}

time_t World::calculateDt(time_t now)
{
	time_t dt = now - lastTime;
	lastTime = now;
	return dt;
}

int World::pushLetterbox()
{
	GameWindow& w = GameWindow::instance();
	Gosu::Graphics& g = w.graphics();

	// Aspect ratio correction.
	rvec2 sz = view->getPhysRes();
	rvec2 lb = -1 * view->getLetterboxOffset();

	g.beginClipping(lb.x, lb.y, sz.x - 2 * lb.x, sz.y - 2 * lb.y);
	int clips = 1;

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

	return clips;
}

void World::popLetterbox(int clips)
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

	ASSERT(doc = Reader::getXMLDoc("world.conf", "dtd/world.dtd"));
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
			startArea = node.content();
		}
		else if (node.is("player")) {
			playerEntity = node.attr("file");
			playerPhase = node.attr("phase");
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
			if (!node.intAttr("x", &startCoords.x) ||
			    !node.intAttr("y", &startCoords.y) ||
			    !node.doubleAttr("z", &startCoords.z))
				return false;
		}
		else if (node.is("viewport")) {
			if (!node.intAttr("width", &viewportSz.x) ||
			    !node.intAttr("height", &viewportSz.y))
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
		.def_readwrite("on_key_down", &World::keydownScript)
		.def_readwrite("on_key_up", &World::keyupScript)
		;
}

