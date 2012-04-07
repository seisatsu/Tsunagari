/*********************************
** Tsunagari Tile Engine        **
** area.cpp                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <algorithm>
#include <math.h>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <Gosu/Graphics.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/Math.hpp>
#include <Gosu/Timing.hpp>

#include "area.h"
#include "common.h"
#include "entity.h"
#include "log.h"
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
		runOnLoads();
	}

	music->setIntro(musicIntro);
	music->setLoop(musicLoop);

	if (onFocusScripts.size()) {
		BOOST_FOREACH(const std::string& script, onFocusScripts) {
			pythonSetGlobal("Area", this);
			Resourcer* rc = Resourcer::instance();
			rc->runPythonScript(script);
		}
	}
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

	// Do any on-screen tile types need to update their animations?
	const icube_t tiles = visibleTiles();
	for (int z = tiles.z1; z < tiles.z2; z++) {
		for (int y = tiles.y1; y < tiles.y2; y++) {
			for (int x = tiles.x1; x < tiles.x2; x++) {
				const Tile& tile = getTile(x, y, z);
				const TileType* type = tile.getType();
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
	player->update(dt);

	if (onUpdateScripts.size()) {
		BOOST_FOREACH(const std::string& script, onUpdateScripts) {
			pythonSetGlobal("Area", this);
			Resourcer* rc = Resourcer::instance();
			rc->runPythonScript(script);
		}
	}

	view->update(dt);
	music->update();
}

AreaPtr Area::reset()
{
	World* world = World::instance();
	AreaPtr newSelf = world->getArea(descriptor, GETAREA_ALWAYS_CREATE);
	if (world->getFocusedArea().get() == this) {
		vicoord c = player->getTileCoords_vi();
		world->focusArea(newSelf, c);
	}
	return newSelf;
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



const Tile& Area::getTile(int x, int y, int z) const
{
	if (loopX)
		x = wrap(0, x, dim.x);
	if (loopY)
		y = wrap(0, y, dim.y);
	return map[z][y][x];
}

const Tile& Area::getTile(int x, int y, double z) const
{
	return getTile(x, y, depthIndex(z));
}

const Tile& Area::getTile(icoord phys) const
{
	return getTile(phys.x, phys.y, phys.z);
}

const Tile& Area::getTile(vicoord virt) const
{
	return getTile(virt2phys(virt));
}

Tile& Area::getTile(int x, int y, int z)
{
	if (loopX)
		x = wrap(0, x, dim.x);
	if (loopY)
		y = wrap(0, y, dim.y);
	return map[z][y][x];
}

Tile& Area::getTile(int x, int y, double z)
{
	return getTile(x, y, depthIndex(z));
}

Tile& Area::getTile(icoord phys)
{
	return getTile(phys.x, phys.y, phys.z);
}

Tile& Area::getTile(vicoord virt)
{
	return getTile(virt2phys(virt));
}

TileType& Area::getTileType(int idx)
{
	return tileTypes[idx];
}



ivec3 Area::getDimensions() const
{
	return dim;
}

ivec2 Area::getTileDimensions() const
{
	return tileDim;
}

icube_t Area::visibleTileBounds() const
{
	rvec2 screen = view->getVirtRes();
	rvec2 off = view->getMapOffset();

	int x1 = (int)floor(off.x / tileDim.x);
	int y1 = (int)floor(off.y / tileDim.y);
	int x2 = (int)ceil((screen.x + off.x) / tileDim.x);
	int y2 = (int)ceil((screen.y + off.y) / tileDim.y);

	return icube(x1, y1, 0, x2, y2, dim.z);
}

icube_t Area::visibleTiles() const
{
	icube_t cube = visibleTileBounds();
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



void Area::runOnLoads()
{
	Resourcer* rc = Resourcer::instance();
	World* world = World::instance();
	std::string onAreaLoadScript = world->getAreaLoadScript();
	if (onAreaLoadScript.size()) {
		pythonSetGlobal("Area", this);
		rc->runPythonScript(onAreaLoadScript);
	}
	BOOST_FOREACH(const std::string& script, onLoadScripts) {
		pythonSetGlobal("Area", this);
		rc->runPythonScript(script);
	}
}

void Area::updateTileAnimations()
{
	const int millis = GameWindow::instance().time();
	BOOST_FOREACH(TileType& type, tileTypes)
		type.anim.updateFrame(millis);
}

void Area::drawTiles() const
{
	const icube_t tiles = visibleTiles();
	for (int z = tiles.z1; z < tiles.z2; z++) {
		double depth = idx2depth[z];
		for (int y = tiles.y1; y < tiles.y2; y++) {
			for (int x = tiles.x1; x < tiles.x2; x++) {
				const Tile& tile = getTile(x, y, z);
				drawTile(tile, x, y, depth);
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
		.def("request_redraw", &Area::requestRedraw)
		.def("tiles",
		    static_cast<Tile& (Area::*) (int, int, double)>
		    (&Area::getTile),
		    return_value_policy<reference_existing_object>())
		.def("in_bounds",
		    static_cast<bool (Area::*) (int, int, double) const>
		    (&Area::inBounds))
		.def("get_tile_type", &Area::getTileType,
		    return_value_policy<reference_existing_object>())
		.def("reset", &Area::reset)
		.def("color_overlay", &Area::setColorOverlay)
		;
}

