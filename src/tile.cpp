/******************************
** Tsunagari Tile Engine     **
** tile.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <boost/foreach.hpp>

#include "area.h"
#include "entity-lua.h"
#include "script.h"
#include "tile.h"
#include "window.h"


Tile::Tile()
	: flags(0x0)
{
}

bool Tile::hasType() const
{
	return types.size();
}

bool Tile::hasFlag(unsigned flag) const
{
	if (flags & flag)
		return true;
	BOOST_FOREACH(const TileType* type, types)
		if (type->flags & flag)
			return true;
	return false;
}

void Tile::onEnterScripts(Resourcer* rc, Entity* triggeredBy)
{
	if (flags & hasOnEnter)
		runScripts(rc, triggeredBy, onEnter, events);
	BOOST_FOREACH(const TileType* type, types)
		if (type->flags & hasOnEnter)
			runScripts(rc, triggeredBy, onEnter, type->events);
}

void Tile::onLeaveScripts(Resourcer* rc, Entity* triggeredBy)
{
	BOOST_FOREACH(const TileType* type, types)
		if (type->flags & hasOnLeave)
			runScripts(rc, triggeredBy, onLeave, type->events);
	if (flags & hasOnLeave)
		runScripts(rc, triggeredBy, onLeave, events);
}

void Tile::runScripts(Resourcer* rc, Entity* entity,
                      const TileEventTrigger trigger,
                      const std::vector<TileEvent>& events)
{
	for (std::vector<TileEvent>::const_iterator i = events.begin(); i != events.end(); i++) {
//	BOOST_FOREACH(const TileEvent& e, events) {
		const TileEvent& e = *i;
		if (e.trigger == trigger) {
			Script s(rc);
			bindEntity(&s, entity, "entity");
			// TODO bindTile(script, tile, "tile");
			s.run(rc, e.script);
		}
	}
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
					BOOST_FOREACH(const TileType* type, tile.types)
						if (type == this)
							return true;
				}
			}
		}
	}
	return false;
}

