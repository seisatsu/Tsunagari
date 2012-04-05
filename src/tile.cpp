/*********************************
** Tsunagari Tile Engine        **
** tile.cpp                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <boost/foreach.hpp>

#include "area.h"
#include "python.h"
#include "python_optional.h"
#include "tile.h"
#include "window.h"

FlagManip::FlagManip(unsigned* flags)
	: flags(flags)
{
}

bool FlagManip::isNowalk() const
{
	return (*flags & TILE_NOWALK) != 0;
}

bool FlagManip::isNowalkPlayer() const
{
	return (*flags & TILE_NOWALK_PLAYER) != 0;
}

bool FlagManip::isNowalkNPC() const
{
	return (*flags & TILE_NOWALK_NPC) != 0;
}

void FlagManip::setNowalk(bool nowalk)
{
	*flags = (*flags & ~TILE_NOWALK) | TILE_NOWALK * nowalk;
}

void FlagManip::setNowalkPlayer(bool nowalk)
{
	*flags = (*flags & ~TILE_NOWALK_PLAYER) | TILE_NOWALK_PLAYER * nowalk;
}

void FlagManip::setNowalkNPC(bool nowalk)
{
	*flags = (*flags & ~TILE_NOWALK_NPC) | TILE_NOWALK_NPC * nowalk;
}


Exit::Exit()
{
}

Exit::Exit(const std::string area, int x, int y, double z)
	: area(area), tile(x, y, z)
{
}


Tile::Tile()
{
}

Tile::Tile(Area* area, int x, int y, int z)
	: area(area), x(x), y(y), z(z), flags(0x0), type(NULL)
{
}

bool Tile::hasFlag(unsigned flag) const
{
	return flags & flag || (type && type->flags & flag);
}

FlagManip Tile::flagManip()
{
	return FlagManip(&flags);
}

Tile& Tile::offset(int x, int y)
{
	return area->getTile(this->x + x, this->y + y, z);
}

void Tile::onEnterScripts(Entity* triggeredBy)
{
	runScripts(triggeredBy, onEnter);
	if (type)
		runScripts(triggeredBy, type->onEnter);
}

void Tile::onLeaveScripts(Entity* triggeredBy)
{
	runScripts(triggeredBy, onLeave);
	if (type)
		runScripts(triggeredBy, type->onLeave);
}

void Tile::onUseScripts(Entity* triggeredBy)
{
	runScripts(triggeredBy, onUse);
	if (type)
		runScripts(triggeredBy, type->onUse);
}

void Tile::runScripts(Entity* triggeredBy,
                      const std::vector<std::string>& events)
{
	BOOST_FOREACH(const std::string& script, events) {
		Resourcer* rc = Resourcer::instance();
		pythonSetGlobal("entity", triggeredBy);
		pythonSetGlobal("tile", this);
		rc->runPythonScript(script);
	}
}

void Tile::setWalkable(bool walkable)
{
	flags = (flags & ~TILE_NOWALK) | TILE_NOWALK * !walkable;
}

bool Tile::getWalkable()
{
	return !hasFlag(TILE_NOWALK);
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

FlagManip TileType::flagManip()
{
	return FlagManip(&flags);
}

bool TileType::visibleIn(const Area& area, const icube_t& tiles) const
{
	for (int z = tiles.z1; z != tiles.z2; z++) {
		for (int y = tiles.y1; y != tiles.y2; y++) {
			for (int x = tiles.x1; x != tiles.x2; x++) {
				icoord pos(x, y, z);
				// Do this check before passing _tiles_ to fn.
				if (area.inBounds(pos)) {
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
	boost::python::class_<FlagManip>
		("FlagManipulator", boost::python::no_init)
		.add_property("nowalk",
			&FlagManip::isNowalk, &FlagManip::setNowalk)
		.add_property("nowalk_player",
			&FlagManip::isNowalkPlayer, &FlagManip::setNowalkPlayer)
		.add_property("nowalk_npc",
			&FlagManip::isNowalkNPC, &FlagManip::setNowalkNPC)
		;
	boost::python::class_<Tile>
		("Tile", boost::python::no_init)
		.def_readonly("x", &Tile::x)
		.def_readonly("y", &Tile::y)
		.def_readonly("z", &Tile::z)
		.def_readwrite("type", &Tile::type)
		.def_readwrite("exit", &Tile::exit)
		.add_property("walkable",
			&Tile::getWalkable, &Tile::setWalkable)
		.add_property("flags", &Tile::flagManip)
		.def("offset", &Tile::offset,
		    boost::python::return_value_policy<
		      boost::python::reference_existing_object
		    >()
		)
		.def("onEnterScripts", &Tile::onEnterScripts)
		.def("onLeaveScripts", &Tile::onLeaveScripts)
		.def("onUseScripts", &Tile::onUseScripts)
		;
	boost::python::class_<TileType>
		("TileType", boost::python::no_init)
		.add_property("flags", &Tile::flagManip)
		;
	boost::python::class_<Exit>
		("Exit", boost::python::init<
			const std::string, int, int, double
		>())
		.def_readwrite("area", &Exit::area)
		.def_readwrite("tile", &Exit::tile)
		;
	boost::python::optional_<Exit>();
}

