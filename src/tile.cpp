/*********************************
** Tsunagari Tile Engine        **
** tile.cpp                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <boost/foreach.hpp>

#include "area.h"
#include "python.h"
#include "python-optional.h"
#include "string.h"
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

static Exit pythonNewExit(std::string area, int x, int y, double z)
{
	return Exit(area, x, y, z);
}


/*
 * FLAGMANIP
 */
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


/*
 * TILEBASE
 */
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

TileType* TileBase::getType() const
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


/*
 * TILE
 */
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

double Tile::getZ()
{
	vicoord vi = area->phys2virt_vi(icoord(x, y, z));
	return vi.z;
}

Exit* Tile::getNormalExit() const
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

Exit* Tile::exitAt(ivec2 dir) const
{
	int idx = ivec2_to_dir(dir);
	return idx == -1 ? NULL : exits[idx];
}

boost::optional<double> Tile::layermodAt(ivec2 dir) const
{
	int idx = ivec2_to_dir(dir);
	return idx == -1 ? boost::optional<double>() : layermods[idx];
}


/*
 * TILETYPE
 */
TileType::TileType()
	: TileBase()
{
}

TileType::TileType(ImageRef& img)
	: TileBase()
{
	anim.addFrame(img);
}

bool TileType::needsRedraw() const
{
	const int millis = GameWindow::instance().time();
	return anim.needsRedraw(millis);
}

/*
 * TILESET
 */
TileSet::TileSet()
{
}

TileSet::TileSet(int width, int height)
	: width(width), height(height)
{
}

void TileSet::add(TileType* type)
{
	types.push_back(type);
}

void TileSet::set(int idx, TileType* type)
{
	types[idx] = type;
}

TileType& TileSet::get(int x, int y)
{
	size_t i = idx(x, y);
	if (i > types.size())
		Log::fatal("TileSet", "index " + itostr((int)i) + " out of bounds");
	return *types[i];
}

int TileSet::getWidth() const
{
	return height;
}

int TileSet::getHeight() const
{
	return width;
}

TileType& TileSet::pyGet(int x, int y)
{
	size_t i = idx(x, y);
	if (i > types.size()) {
		PyErr_SetString(PyExc_IndexError,
			"TileSet::at(): x, y index out of range");
		boost::python::throw_error_already_set();
	}
	return *types[i];
}

size_t TileSet::idx(int x, int y) const
{
	return y * width + x;
}


/*
 * PYTHON
 */
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
		.add_property("flag", &TileBase::flagManip)
		.add_property("type",
		    make_function(
		      static_cast<TileType* (TileBase::*) () const>
		        (&TileBase::getType),
		      return_value_policy<reference_existing_object>()),
		    &TileBase::setType)
		.def("onenter_scripts", &TileBase::onEnterScripts)
		.def("onleave_scripts", &TileBase::onLeaveScripts)
		.def("onuse_scripts", &TileBase::onUseScripts)
		;
	class_<Tile, bases<TileBase> > ("Tile", no_init)
		.def_readonly("area", &Tile::area)
		.def_readonly("x", &Tile::x)
		.def_readonly("y", &Tile::y)
		.add_property("z", &Tile::getZ)
		.add_property("exit",
		    make_function(
		      static_cast<Exit* (Tile::*) () const>
		        (&Tile::getNormalExit),
		      return_value_policy<reference_existing_object>()),
		    &Tile::setNormalExit)
		.def("offset", &Tile::offset,
		    return_value_policy<reference_existing_object>())
		;
	class_<TileType, bases<TileBase> > ("TileType", no_init)
		;
	class_<TileSet> ("TileSet", no_init)
		.add_property("width", &TileSet::getWidth)
		.add_property("height", &TileSet::getHeight)
		.def("at", &TileSet::pyGet,
		    return_value_policy<reference_existing_object>())
		;
	class_<Exit> ("Exit", no_init)
		.def_readwrite("area", &Exit::area)
		.def_readwrite("coords", &Exit::coords)
		;
	pythonAddFunction("new_exit", pythonNewExit);
}

