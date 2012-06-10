/*********************************
** Tsunagari Tile Engine        **
** area.cpp                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <algorithm>
#include <math.h>
#include <stdlib.h> // for exit(1) on fatal
#include <vector>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <Gosu/Graphics.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/Math.hpp>
#include <Gosu/Timing.hpp>

#include "area.h"
#include "entity.h"
#include "log.h"
#include "npc.h"
#include "overlay.h"
#include "python.h"
#include "resourcer.h"
#include "tile.h"
#include "window.h"
#include "world.h"

#define ASSERT(x)  if (!(x)) return false

/* NOTE: In the TMX map format used by Tiled, tileset tiles start counting
         their Y-positions from 0, while layer tiles start counting from 1. I
         can't imagine why the author did this, but we have to take it into
         account.
*/

template<class T>
static T wrap(T min, T value, T max)
{
	while (value < min)
		value += max;
	return value % max;
}

Area::Area(Viewport* view,
           Player* player,
           Music* music,
           const std::string& descriptor)
	: view(view),
	  player(player),
	  music(music),
	  colorOverlay(0, 0, 0, 0),
	  dim(0, 0, 0),
	  tileDim(0, 0),
	  loopX(false), loopY(false),
	  beenFocused(false),
	  redraw(true),
	  descriptor(descriptor)
{
}

Area::~Area()
{
}

bool Area::init()
{
	// Abstract method.
	return false;
}

void Area::focus()
{
	if (!beenFocused) {
		beenFocused = true;
		runLoadScripts();
	}

	music->setIntro(musicIntro);
	music->setLoop(musicLoop);

	pythonSetGlobal("Area", this);
	focusScript.invoke();
}

void Area::buttonDown(const Gosu::Button btn)
{
	if (btn == Gosu::kbRight)
		player->startMovement(ivec2(1, 0));
	else if (btn == Gosu::kbLeft)
		player->startMovement(ivec2(-1, 0));
	else if (btn == Gosu::kbUp)
		player->startMovement(ivec2(0, -1));
	else if (btn == Gosu::kbDown)
		player->startMovement(ivec2(0, 1));
	else if (btn == Gosu::kbSpace)
		player->useTile();
}

void Area::buttonUp(const Gosu::Button btn)
{
	if (btn == Gosu::kbRight)
		player->stopMovement(ivec2(1, 0));
	else if (btn == Gosu::kbLeft)
		player->stopMovement(ivec2(-1, 0));
	else if (btn == Gosu::kbUp)
		player->stopMovement(ivec2(0, -1));
	else if (btn == Gosu::kbDown)
		player->stopMovement(ivec2(0, 1));
}

void Area::draw()
{
	updateTileAnimations();
	drawTiles();
	drawEntities();
	drawColorOverlay();
	redraw = false;
}

bool Area::needsRedraw() const
{
	if (redraw)
		return true;
	if (player->needsRedraw())
		return true;

	BOOST_FOREACH(Entity* e, entities)
		if (e->needsRedraw())
			return true;

	// Do any on-screen tile types need to update their animations?
	const icube tiles = visibleTiles();
	for (int z = tiles.z1; z < tiles.z2; z++) {
		for (int y = tiles.y1; y < tiles.y2; y++) {
			for (int x = tiles.x1; x < tiles.x2; x++) {
				const Tile* tile = getTile(x, y, z);
				const TileType* type = tile->getType();
				if (type && type->needsRedraw())
					return true;
			}
		}
	}
	return false;
}

void Area::requestRedraw()
{
	redraw = true;
}

void Area::update(unsigned long dt)
{
	pythonSetGlobal("Area", this);
	updateScript.invoke();

	pythonSetGlobal("Area", this);
	player->update(dt);

	BOOST_FOREACH(Entity* e, entities)
		e->update(dt);

	view->update(dt);
	music->update();
}

void Area::setColorOverlay(int r, int g, int b, int a)
{
	using namespace Gosu;

	if (0 <= r && r < 256 &&
	    0 <= g && g < 256 &&
	    0 <= b && b < 256 &&
	    0 <= a && a < 256) {
		Color::Channel ac = (Color::Channel)a;
		Color::Channel rc = (Color::Channel)r;
		Color::Channel gc = (Color::Channel)g;
		Color::Channel bc = (Color::Channel)b;
		colorOverlay = Color(ac, rc, gc, bc);
		redraw = true;
	}
	else {
		PyErr_Format(PyExc_ValueError,
			"Area::color_overlay() arguments must be "
			"between 0 and 255");
	}
}



const Tile* Area::getTile(int x, int y, int z) const
{
	if (loopX)
		x = wrap(0, x, dim.x);
	if (loopY)
		y = wrap(0, y, dim.y);
	if (inBounds(x, y, z))
		return &map[z][y][x];
	else
		return NULL;
}

const Tile* Area::getTile(int x, int y, double z) const
{
	return getTile(x, y, depthIndex(z));
}

const Tile* Area::getTile(icoord phys) const
{
	return getTile(phys.x, phys.y, phys.z);
}

const Tile* Area::getTile(vicoord virt) const
{
	return getTile(virt2phys(virt));
}

const Tile* Area::getTile(rcoord virt) const
{
	return getTile(virt2phys(virt));
}

Tile* Area::getTile(int x, int y, int z)
{
	if (loopX)
		x = wrap(0, x, dim.x);
	if (loopY)
		y = wrap(0, y, dim.y);
	if (inBounds(x, y, z))
		return &map[z][y][x];
	else
		return NULL;
}

Tile* Area::getTile(int x, int y, double z)
{
	return getTile(x, y, depthIndex(z));
}

Tile* Area::getTile(icoord phys)
{
	return getTile(phys.x, phys.y, phys.z);
}

Tile* Area::getTile(vicoord virt)
{
	return getTile(virt2phys(virt));
}

Tile* Area::getTile(rcoord virt)
{
	return getTile(virt2phys(virt));
}

TileSet* Area::getTileSet(const std::string& imagePath)
{
	std::map<std::string, TileSet>::iterator it;
	it = tileSets.find(imagePath);
	if (it == tileSets.end()) {
		Log::err("Area", "tileset " + imagePath + " not found");
		return NULL;
	}
	return &tileSets[imagePath];
}


ivec3 Area::getDimensions() const
{
	return dim;
}

ivec2 Area::getTileDimensions() const
{
	return tileDim;
}

icube Area::visibleTileBounds() const
{
	rvec2 screen = view->getVirtRes();
	rvec2 off = view->getMapOffset();

	int x1 = (int)floor(off.x / tileDim.x);
	int y1 = (int)floor(off.y / tileDim.y);
	int x2 = (int)ceil((screen.x + off.x) / tileDim.x);
	int y2 = (int)ceil((screen.y + off.y) / tileDim.y);

	return icube(x1, y1, 0, x2, y2, dim.z);
}

icube Area::visibleTiles() const
{
	icube cube = visibleTileBounds();
	if (!loopX) {
		cube.x1 = std::max(cube.x1, 0);
		cube.x2 = std::min(cube.x2, dim.x);
	}
	if (!loopY) {
		cube.y1 = std::max(cube.y1, 0);
		cube.y2 = std::min(cube.y2, dim.y);
	}
	return cube;
}

bool Area::inBounds(int x, int y, int z) const
{
	return ((loopX || (0 <= x && x < dim.x)) &&
		(loopY || (0 <= y && y < dim.y)) &&
		          0 <= z && z < dim.z);
}

bool Area::inBounds(int x, int y, double z) const
{
	return inBounds(x, y, depthIndex(z));
}

bool Area::inBounds(icoord phys) const
{
	return inBounds(phys.x, phys.y, phys.z);
}

bool Area::inBounds(vicoord virt) const
{
	return inBounds(virt2phys(virt));
}

bool Area::inBounds(rcoord virt) const
{
	return inBounds(virt2phys(virt));
}

bool Area::inBounds(Entity* ent) const
{
	return inBounds(ent->getPixelCoord());
}



bool Area::loopsInX() const
{
	return loopX;
}

bool Area::loopsInY() const
{
	return loopY;
}

const std::string Area::getDescriptor() const
{
	return descriptor;
}

Entity* Area::spawnNPC(const std::string& descriptor,
	int x, int y, double z, const std::string& phase)
{
	Entity* e = new NPC();
	if (!e->init(descriptor)) {
		// Error logged.
		delete e;
		return NULL;
	}
	e->setArea(this);
	if (!e->setPhase(phase)) {
		// Error logged.
		delete e;
		return NULL;
	}
	e->setTileCoords(x, y, z);
	insert(e);
	return e;
}

Entity* Area::spawnOverlay(const std::string& descriptor,
	int x, int y, double z, const std::string& phase)
{
	Entity* e = new Overlay();
	if (!e->init(descriptor)) {
		// Error logged.
		delete e;
		return NULL;
	}
	e->setArea(this);
	if (!e->setPhase(phase)) {
		// Error logged.
		delete e;
		return NULL;
	}
	e->setTileCoords(x, y, z);
	// XXX: e->leaveTile(); // Overlays don't consume tiles.

	insert(e);
	return e;
}

void Area::insert(Entity* e)
{
	entities.insert(e);
}

void Area::erase(Entity* e)
{
	entities.erase(e);
}



vicoord Area::phys2virt_vi(icoord phys) const
{
	return vicoord(phys.x, phys.y, indexDepth(phys.z));
}

rcoord Area::phys2virt_r(icoord phys) const
{
	return rcoord(
		(double)phys.x * tileDim.x,
		(double)phys.y * tileDim.y,
		indexDepth(phys.z)
	);
}

icoord Area::virt2phys(vicoord virt) const
{
	return icoord(virt.x, virt.y, depthIndex(virt.z));
}

icoord Area::virt2phys(rcoord virt) const
{
	return icoord(
		(int)(virt.x / tileDim.x),
		(int)(virt.y / tileDim.y),
		depthIndex(virt.z)
	);
}

rcoord Area::virt2virt(vicoord virt) const
{
	return rcoord(
		(double)virt.x * tileDim.x,
		(double)virt.y * tileDim.y,
		virt.z
	);
}

vicoord Area::virt2virt(rcoord virt) const
{
	return vicoord(
		(int)virt.x / tileDim.x,
		(int)virt.y / tileDim.y,
		virt.z
	);
}


int Area::depthIndex(double depth) const
{
	return depth2idx.find(depth)->second;
}

double Area::indexDepth(int idx) const
{
	return idx2depth[idx];
}



void Area::runLoadScripts()
{
	World* world = World::instance();
	world->runAreaLoadScript(this);

	pythonSetGlobal("Area", this);
	loadScript.invoke();
}

void Area::updateTileAnimations()
{
	const int millis = GameWindow::instance().time();
	BOOST_FOREACH(tilesets_t::value_type& pair, tileSets) {
		TileSet& set = pair.second;
		int w = set.getWidth();
		int h = set.getHeight();
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				TileType* type = set.get(x, y);
				type->anim.updateFrame(millis);
			}
		}
	}
}

void Area::drawTiles() const
{
	const icube tiles = visibleTiles();
	for (int z = tiles.z1; z < tiles.z2; z++) {
		double depth = idx2depth[z];
		for (int y = tiles.y1; y < tiles.y2; y++) {
			for (int x = tiles.x1; x < tiles.x2; x++) {
				const Tile* tile = getTile(x, y, z);
				drawTile(*tile, x, y, depth);
			}
		}
	}
}

void Area::drawTile(const Tile& tile, int x, int y, double depth) const
{
	const TileType* type = (TileType*)tile.parent;
	if (type) {
		const Gosu::Image* img = type->anim.frame();
		if (img)
			img->draw((double)x*img->width(),
				  (double)y*img->height(), depth);
	}
}

void Area::drawEntities()
{
	BOOST_FOREACH(Entity* e, entities) {
		e->draw();
	}
	player->draw();
}

void Area::drawColorOverlay()
{
	if (colorOverlay.alpha() != 0) {
		GameWindow& window = GameWindow::instance();
		Gosu::Color c = colorOverlay;
		int x = window.width();
		int y = window.height();
		window.graphics().drawQuad(
			0, 0, c,
			x, 0, c,
			x, y, c,
			0, y, c,
			750
		);
	}
}

boost::python::tuple Area::pyGetDimensions()
{
	using namespace boost::python;

	list zs;
	BOOST_FOREACH(double dep, idx2depth)
		zs.append(dep);
	return make_tuple(dim.x, dim.y, zs);
}

void exportArea()
{
	using namespace boost::python;

	class_<Area>("Area", no_init)
		.add_property("descriptor", &Area::getDescriptor)
		.add_property("dimensions", &Area::pyGetDimensions)
		.def("redraw", &Area::requestRedraw)
		.def("tileset", &Area::getTileSet,
		    return_value_policy<reference_existing_object>())
		.def("tile",
		    static_cast<Tile* (Area::*) (int, int, double)>
		    (&Area::getTile),
		    return_value_policy<reference_existing_object>())
		.def("in_bounds",
		    static_cast<bool (Area::*) (int, int, double) const>
		    (&Area::inBounds))
		.def("color_overlay", &Area::setColorOverlay)
		.def("new_npc", &Area::spawnNPC,
		    return_value_policy<reference_existing_object>())
		.def("new_overlay", &Area::spawnOverlay,
		    return_value_policy<reference_existing_object>())
		.def_readwrite("on_focus", &Area::focusScript)
		.def_readwrite("on_update", &Area::updateScript)
		;
}

