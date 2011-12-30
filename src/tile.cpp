/******************************
** Tsunagari Tile Engine     **
** tile.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <boost/foreach.hpp>

#include "area.h"
#include "python.h"
#include "tile.h"
#include "window.h"


Tile::Tile()
	: type(NULL), flags(0x0)
{
}

bool Tile::hasFlag(unsigned flag) const
{
	return flags & flag || (type && type->flags & flag);
}

void Tile::onEnterScripts(Resourcer* rc, Entity* triggeredBy)
{
	runScripts(rc, triggeredBy, hasOnEnter, onEnter);
}

void Tile::onLeaveScripts(Resourcer* rc, Entity* triggeredBy)
{
	runScripts(rc, triggeredBy, hasOnLeave, onLeave);
}

void Tile::onUseScripts(Resourcer* rc, Entity* triggeredBy)
{
	runScripts(rc, triggeredBy, hasOnUse, onUse);
}

void Tile::runScripts(Resourcer* rc, Entity* entity,
                      TileFlags flag, TileEventTrigger trigger)
{
	if (type && type->flags & flag)
		runScriptGroup(rc, entity, trigger, type->events);
	if (flags & flag)
		runScriptGroup(rc, entity, trigger, events);
}

void Tile::runScriptGroup(Resourcer* rc, Entity* entity,
                      TileEventTrigger trigger,
                      const std::vector<TileEvent>& events)
{
	BOOST_FOREACH(const TileEvent& event, events) {
		if (event.trigger == trigger)
			runScript(rc, entity, event.script);
	}
}

void Tile::runScript(Resourcer* rc, Entity* entity, const std::string& script)
{
	pythonSetGlobal("entity", entity);
	pythonSetGlobal("tile", this);
	rc->runPythonScript(script);
}


TileType::TileType()
	: flags(0x0)
{
}

TileType::TileType(TiledImage& img)
	: flags(0x0)
{
	anim.addFrame(img.front());
	img.pop_front();
}

bool TileType::needsRedraw(const Area& area) const
{
	const int millis = GameWindow::getWindow().time();
	return anim.needsRedraw(millis) &&
	       visibleIn(area, area.visibleTiles());
}

bool TileType::visibleIn(const Area& area, const icube_t& tiles) const
{
	for (int z = tiles.z1; z != tiles.z2; z++) {
		for (int y = tiles.y1; y != tiles.y2; y++) {
			for (int x = tiles.x1; x != tiles.x2; x++) {
				icoord pos(x, y, z);
				// Do this check before passing _tiles_ to fn.
				if (area.tileExists(pos)) {
					const Tile& tile = area.getTile(pos);
					if (tile.type == this)
						return true;
				}
			}
		}
	}
	return false;
}


void exportTile()
{
	boost::python::class_<Tile>("Tile", boost::python::no_init)
		.def("hasFlag", &Tile::hasFlag)
		.def("onEnterScripts", &Tile::onEnterScripts)
		.def("onLeaveScripts", &Tile::onLeaveScripts);
}

void exportTileType()
{
	boost::python::class_<TileType>("TileType", boost::python::no_init);
}

void exportDoor()
{
	boost::python::class_<Door>("Door", boost::python::no_init);
}

