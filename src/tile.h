/***************************************
** Tsunagari Tile Engine              **
** tile.h                             **
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

#ifndef TILE_H
#define TILE_H

#include <string>
#include <vector>

#include <boost/optional.hpp>

class Tile;
class TileType;
class TileSet;

#include "area.h"
#include "animation.h"
#include "reader.h" // for TiledImage
#include "scriptinst.h"
#include "vec.h"

class Area;
class Entity;
class TileType;

//! List of possible flags that can be attached to a tile.
/*!
	Flags are attached to tiles and denote special behavior for
	the tile they are bound to.

	see AreaTMX::splitTileFlags().
*/

/**
 * TILE_NOWALK
 * Neither the player nor NPCs can walk here.
 */
#define TILE_NOWALK          0x001

/**
 * TILE_NOWALK_PLAYER
 * The player cannot walk here. NPCs can, though.
 */
#define TILE_NOWALK_PLAYER   0x002

/**
 * TILE_NOWALK_NPC
 * NPCs cannot walk here. The player can, though.
 */
#define TILE_NOWALK_NPC      0x004

/**
 * TILE_NOWALK_EXIT
 * This Tile is an Exit. Please take appropriate action when entering this Tile,
 * usually by transferring to another Area.
 *
 * This flag is not carried by actual Tiles, but can instead be flipped in an
 * Entity's "exempt" flag which will be read elsewhere in the engine.
 */
#define TILE_NOWALK_EXIT     0x008

/**
 * TILE_NOWALK_AREA_BOUND
 * This Tile is at the edge of an Area. If you step here, please handle it
 * appropriately.
 *
 * (Usually if one moves off a map bound, one will either transfer to another
 * Area, or will be destroyed.)
 *
 * This flag is not carried by actual Tiles, but can instead be flipped in an
 * Entity's "exempt" flag which will be read elsewhere in the engine.
 */
#define TILE_NOWALK_AREA_BOUND 0x016


/**
 * Types of exits.
 */
enum ExitDirection {
	/**
	 * An Exit that is taken upon arriving at the Tile.
	 */
	EXIT_NORMAL,
	/**
	 * An Exit that is taken when leaving in the upwards
	 * direction from a Tile.
	 */
	EXIT_UP,
	/**
	 * An Exit that is taken when leaving in the downwards
	 * direction from a Tile.
	 */
	EXIT_DOWN,
	/**
	 * An Exit that is taken when leaving to the left from
	 * a Tile.
	 */
	EXIT_LEFT,
	/**
	 * An Exit that is taken when leaving to the right from
	 * a Tile.
	 */
	EXIT_RIGHT,
	EXITS_LENGTH
};

/**
 * Independant object that can manipulate a Tile's flags.
 */
class FlagManip
{
public:
	FlagManip(unsigned* flags);

	bool isNowalk() const;
	bool isNowalkPlayer() const;
	bool isNowalkNPC() const;
	bool isNowalkExit() const;
	bool isNowalkAreaBound() const;

	void setNowalk(bool nowalk);
	void setNowalkPlayer(bool nowalk);
	void setNowalkNPC(bool nowalk);
	void setNowalkExit(bool nowalk);
	void setNowalkAreaBound(bool nowalk);

private:
	unsigned* flags;
};

//! Convenience trigger for inter-area teleportation.
/*!
	Tiles with a exit trigger attached can teleport the player to a
	new area in the World. The Exit struct contains the destination
	area and coordinates.
*/
class Exit {
public:
	Exit();
	Exit(const std::string area, int x, int y, double z);

public:
	std::string area;
	vicoord coords;
};

class TileBase
{
public:
	TileBase();

	FlagManip flagManip();

	//! Determines whether this tile or one of its parent types embodies a
	//! flag.
	bool hasFlag(unsigned flag) const;

	TileType* getType() const;
	void setType(TileType* type);

	void runEnterScript(Entity* triggeredBy);
	void runLeaveScript(Entity* triggeredBy);
	void runUseScript(Entity* triggeredBy);

private:
	void runScript(Entity* triggeredBy, ScriptInst& script);

public:
	TileBase* parent;
	unsigned flags;
	boost::optional<ScriptInst> enterScript, leaveScript, useScript;
};

//! Contains properties unique to this tile.
/*!
	This struct contains local tile properties for a single tile in
	the area. As opposed to global properties which apply to all
	tiles of the same type, these properties will only apply to one
	tile.
*/
class Tile : public TileBase
{
public:
	Tile(); // Should not be used. Wanted by std::containers.
	Tile(Area* area, int x, int y, int z);

	/**
	 * Gets the correct destination for an Entity wanting to
	 * move off of this tile in <code>facing</code>
	 * direction.
	 *
	 * This call is necessary to handle layermod.
	 *
	 * @param here    area-space coordinate for this Tile
	 * @param facing  facing vector
	 */
	icoord moveDest(icoord here, ivec2 facing) const;
	Tile* offset(int x, int y) const;

	double getZ() const;

	Exit* getNormalExit() const;
	void setNormalExit(Exit exit);

	Exit* exitAt(ivec2 dir) const;
	boost::optional<double> layermodAt(ivec2 dir) const;

public:
	Area* area;

	/*
	 * The grid-space coordinates of this Tile.
	 *
	 * Looping Areas utilize area-space components. These
	 * cannot be losslessly transformed into area-space.
	 */
	int x, y, z;
	Exit* exits[EXITS_LENGTH];
	boost::optional<double> layermods[EXITS_LENGTH];
	int entCnt; //!< Number of entities on this Tile.
};

//! Contains the properties shared by all tiles of a certain type.
/*!
	This struct contains global tile properties for a tile of a
	certain type. As opposed to local properties for a single tile,
	all tiles of this type will share the defined characteristics.
*/
class TileType : public TileBase
{
public:
	TileType();
	TileType(ImageRef& img);

	//! Returns true if onscreen and we need to update our animation.
	bool needsRedraw() const;

public:
	Animation anim; //! Graphics for tiles of this type.
	std::vector<Tile*> allOfType;
};

class TileSet
{
public:
	TileSet();
	TileSet(int width, int height);

	void add(TileType* type);
	void set(int idx, TileType* type);
	TileType* get(int x, int y);
	int getWidth() const;
	int getHeight() const;

private:
	size_t idx(int x, int y) const;

	std::vector<TileType*> types;
	int width, height;
};

void exportTile();

#endif

