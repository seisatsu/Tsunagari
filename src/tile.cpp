/***************************************
** Tsunagari Tile Engine              **
** tile.cpp                           **
** Copyright 2011-2013 PariahSoft LLC **
***************************************/

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

#include <stdlib.h> // for exit(1) on fatal

#include "area.h"
#include "formatter.h"
#include "log.h"
#include "python.h"
#include "python-bindings-template.cpp"
#include "string.h"
#include "tile.h"
#include "world.h"

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

bool FlagManip::isNowalkExit() const
{
	return (*flags & TILE_NOWALK_EXIT) != 0;
}

bool FlagManip::isNowalkAreaBound() const
{
	return (*flags & TILE_NOWALK_AREA_BOUND) != 0;
}

void FlagManip::setNowalk(bool nowalk)
{
	*flags &= ~TILE_NOWALK;
	*flags |= TILE_NOWALK * nowalk;
}

void FlagManip::setNowalkPlayer(bool nowalk)
{
	*flags &= ~TILE_NOWALK_PLAYER;
	*flags |= TILE_NOWALK_PLAYER * nowalk;
}

void FlagManip::setNowalkNPC(bool nowalk)
{
	*flags &= ~TILE_NOWALK_NPC;
	*flags |= TILE_NOWALK_NPC * nowalk;
}

void FlagManip::setNowalkExit(bool nowalk)
{
	*flags &= ~TILE_NOWALK_EXIT;
	*flags |= TILE_NOWALK_EXIT * nowalk;
}

void FlagManip::setNowalkAreaBound(bool nowalk)
{
	*flags &= ~TILE_NOWALK_AREA_BOUND;
	*flags |= TILE_NOWALK_AREA_BOUND * nowalk;
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

FlagManip TileBase::flagManip()
{
	return FlagManip(&flags);
}

bool TileBase::hasFlag(unsigned flag) const
{
	return flags & flag || (parent && parent->hasFlag(flag));
}

TileType* TileBase::getType() const
{
	return (TileType*)parent;
}

void TileBase::setType(TileType* type)
{
	parent = type;
}

void TileBase::runEnterScript(Entity* triggeredBy)
{
	if (enterScript)
		runScript(triggeredBy, enterScript);
	if (parent)
		parent->runEnterScript(triggeredBy);
}

void TileBase::runLeaveScript(Entity* triggeredBy)
{
	if (leaveScript)
		runScript(triggeredBy, leaveScript);
	if (parent)
		parent->runLeaveScript(triggeredBy);
}

void TileBase::runUseScript(Entity* triggeredBy)
{
	if (useScript)
		runScript(triggeredBy, useScript);
	if (parent)
		parent->runUseScript(triggeredBy);
}

void TileBase::runScript(Entity* triggeredBy, ScriptRef& script)
{
	pythonSetGlobal("Entity", triggeredBy);
	pythonSetGlobal("Tile", this);
	script->invoke();
}


/*
 * TILE
 */
Tile::Tile()
	: entCnt(0)
{
}

Tile::Tile(Area* area, int x, int y, int z)
	: TileBase(), area(area), x(x), y(y), z(z), entCnt(0)
{
	memset(exits, 0, sizeof(exits));
	memset(layermods, 0, sizeof(layermods));
}

icoord Tile::moveDest(icoord here, ivec2 facing) const
{
	icoord dest = here + icoord(facing.x, facing.y, 0);

	double* layermod = layermodAt(facing);
	if (layermod)
		dest = area->virt2phys(vicoord(dest.x, dest.y, *layermod));
	return dest;
}

Tile* Tile::offset(int x, int y) const
{
	return area->getTile(this->x + x, this->y + y, z);
}

double Tile::getZ() const
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

double* Tile::layermodAt(ivec2 dir) const
{
	int idx = ivec2_to_dir(dir);
	return idx == -1 ? NULL : layermods[idx];
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
	anim = Animation(img);
}

bool TileType::needsRedraw() const
{
	time_t now = World::instance()->time();
	return anim.needsRedraw(now);
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

TileType* TileSet::get(int x, int y)
{
	size_t i = idx(x, y);
	if (i > types.size()) {
		Log::err("TileSet",
			Formatter("get(%, %): out of bounds") % x % y);
		return NULL;
	}
	return types[i];
}

int TileSet::getWidth() const
{
	return height;
}

int TileSet::getHeight() const
{
	return width;
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
		.add_property("nowalk_exit",
			&FlagManip::isNowalkExit, &FlagManip::setNowalkExit)
		.add_property("nowalk_area_bound",
			&FlagManip::isNowalkAreaBound,
			&FlagManip::setNowalkAreaBound)
		;
	class_<TileBase> ("TileBase", no_init)
		.add_property("flag", &TileBase::flagManip)
		.add_property("type",
		    make_function(
		      static_cast<TileType* (TileBase::*) () const>
		        (&TileBase::getType),
		      return_value_policy<reference_existing_object>()),
		    &TileBase::setType)
//		.def_readwrite("on_enter", &TileBase::enterScript)
//		.def_readwrite("on_leave", &TileBase::leaveScript)
//		.def_readwrite("on_use", &TileBase::useScript)
		.def("run_enter_script", &TileBase::runEnterScript)
		.def("run_leave_script", &TileBase::runLeaveScript)
		.def("run_use_script", &TileBase::runUseScript)
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
		.def_readonly("nentities", &Tile::entCnt)
		.def("offset", &Tile::offset,
		    return_value_policy<reference_existing_object>())
		;
	class_<TileType, bases<TileBase> > ("TileType", no_init)
		;
	class_<TileSet> ("TileSet", no_init)
		.add_property("width", &TileSet::getWidth)
		.add_property("height", &TileSet::getHeight)
		.def("at", &TileSet::get,
		    return_value_policy<reference_existing_object>())
		;
	class_<Exit> ("Exit", no_init)
		.def_readwrite("area", &Exit::area)
		.def_readwrite("coords", &Exit::coords)
		;
	pythonAddFunction("new_exit", pythonNewExit);
}

