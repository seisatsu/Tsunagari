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

static int ivec2_to_dir(ivec2 v)
{
	switch (v.x) {
	case -1:
		return v.y == 0 ? EXIT_LEFT : -1;
	case 0:
		switch (v.y) {
		case -1:
			return EXIT_UP;
		case 0:
			return EXIT_NORMAL;
		case 1:
			return EXIT_DOWN;
		default:
			return -1;
		}
		break;
	case 1:
		return v.y == 0 ? EXIT_RIGHT : -1;
	default:
		return -1;
	}
}

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
	: area(area), coords(x, y, z)
{
}


TileBase::TileBase()
	: parent(NULL), flags(0x0)
{
}

bool TileBase::hasFlag(unsigned flag) const
{
	return flags & flag || (parent && parent->hasFlag(flag));
}

FlagManip TileBase::flagManip()
{
	return FlagManip(&flags);
}

TileType* TileBase::getType()
{
	return (TileType*)parent;
}

void TileBase::setType(TileType* type)
{
	parent = type;
}

void TileBase::onEnterScripts(Entity* triggeredBy)
{
	runScripts(triggeredBy, onEnter);
	if (parent)
		parent->onEnterScripts(triggeredBy);
}

void TileBase::onLeaveScripts(Entity* triggeredBy)
{
	runScripts(triggeredBy, onLeave);
	if (parent)
		parent->onLeaveScripts(triggeredBy);
}

void TileBase::onUseScripts(Entity* triggeredBy)
{
	runScripts(triggeredBy, onUse);
	if (parent)
		parent->onUseScripts(triggeredBy);
}

void TileBase::runScripts(Entity* triggeredBy,
                      const std::vector<std::string>& events)
{
	BOOST_FOREACH(const std::string& script, events) {
		Resourcer* rc = Resourcer::instance();
		pythonSetGlobal("Entity", triggeredBy);
		pythonSetGlobal("Tile", this);
		rc->runPythonScript(script);
	}
}

Tile::Tile()
{
}

Tile::Tile(Area* area, int x, int y, int z)
	: TileBase(), area(area), x(x), y(y), z(z)
{
	memset(exits, 0, sizeof(exits));
	memset(layermods, 0, sizeof(layermods));
}

Tile& Tile::offset(int x, int y)
{
	return area->getTile(this->x + x, this->y + y, z);
}

Exit* Tile::getNormalExit()
{
	return exits[EXIT_NORMAL];
}

void Tile::setNormalExit(Exit exit)
{
	Exit** norm = &exits[EXIT_NORMAL];
	if (*norm)
		delete *norm;
	*norm = new Exit(exit);
}

Exit* Tile::exitAt(ivec2 dir)
{
	int idx = ivec2_to_dir(dir);
	return idx == -1 ? NULL : exits[idx];
}

boost::optional<double> Tile::layermodAt(ivec2 dir)
{
	int idx = ivec2_to_dir(dir);
	return idx == -1 ? boost::optional<double>() : layermods[idx];
}

TileType::TileType()
	: TileBase()
{
}

TileType::TileType(TiledImage& img)
	: TileBase()
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
				if (area.inBounds(pos)) {
					const Tile& tile = area.getTile(pos);
					if (tile.parent == this)
						return true;
				}
			}
		}
	}
	return false;
}



Exit pythonNewExit(std::string area, int x, int y, double z)
{
	return Exit(area, x, y, z);
}


void exportTile()
{
	using namespace boost::python;

	class_<FlagManip> ("FlagManipulator", no_init)
		.add_property("nowalk",
			&FlagManip::isNowalk, &FlagManip::setNowalk)
		.add_property("nowalk_player",
			&FlagManip::isNowalkPlayer, &FlagManip::setNowalkPlayer)
		.add_property("nowalk_npc",
			&FlagManip::isNowalkNPC, &FlagManip::setNowalkNPC)
		;
	class_<TileBase> ("TileBase", no_init)
		.add_property("flags", &TileBase::flagManip)
		.add_property("type",
		    make_function(
		      static_cast<TileType* (TileBase::*) ()>
		      (&TileBase::getType),
		      return_value_policy<reference_existing_object>()),
		    &TileBase::setType)
		.def("onEnterScripts", &TileBase::onEnterScripts)
		.def("onLeaveScripts", &TileBase::onLeaveScripts)
		.def("onUseScripts", &TileBase::onUseScripts)
		;
	class_<Tile, bases<TileBase> > ("Tile", no_init)
		.def_readonly("area", &Tile::area)
		.def_readonly("x", &Tile::x)
		.def_readonly("y", &Tile::y)
		.def_readonly("z", &Tile::z)
		.add_property("exit",
		    make_function(
		      static_cast<Exit* (Tile::*) ()> (&Tile::getNormalExit),
		      return_value_policy<reference_existing_object>()),
		    &Tile::setNormalExit)
		.def("offset", &Tile::offset,
		    return_value_policy<reference_existing_object>())
		;
	class_<TileType, bases<TileBase> > ("TileType", no_init)
		;
	class_<Exit> ("Exit", no_init)
		.def_readwrite("area", &Exit::area)
		.def_readwrite("coords", &Exit::coords)
		;
	pythonAddFunction("newExit", pythonNewExit);
}

