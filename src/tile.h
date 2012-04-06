/*********************************
** Tsunagari Tile Engine        **
** tile.h                       **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef TILE_H
#define TILE_H

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include "animation.h"
#include "common.h"
#include "resourcer.h" // for TiledImage

class Area;
class Entity;
class TileType;

//! List of possible flags that can be attached to a tile.
/*!
	Flags are attached to tiles and denote special behavior for
	the tile they are bound to.

	see AreaTMX::splitTileFlags().
*/
#define TILE_NOWALK        0x001
#define TILE_NOWALK_PLAYER 0x002
#define TILE_NOWALK_NPC    0x004

// Indexes into Exit and layermod arrays found in class Tile.
enum ExitDirection {
	EXIT_NORMAL,
	EXIT_UP,
	EXIT_DOWN,
	EXIT_LEFT,
	EXIT_RIGHT
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

	void setNowalk(bool nowalk);
	void setNowalkPlayer(bool nowalk);
	void setNowalkNPC(bool nowalk);

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

	//! Determines whether this tile or one of its parent types embodies a
	//! flag.
	bool hasFlag(unsigned flag) const;
	FlagManip flagManip();

	TileType* getType() const;
	void setType(TileType* type);

	void onEnterScripts(Entity* triggeredBy);
	void onLeaveScripts(Entity* triggeredBy);
	void onUseScripts(Entity* triggeredBy);

private:
	//! Runs scripts in a vector.
	void runScripts(Entity* triggeredBy,
		const std::vector<std::string>& events);

public:
	TileBase* parent;
	unsigned flags;
	std::vector<std::string> onEnter, onLeave, onUse;
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

	Tile& offset(int x, int y);

	double getZ();

	Exit* getNormalExit() const;
	void setNormalExit(Exit exit);

	Exit* exitAt(ivec2 dir) const;
	boost::optional<double> layermodAt(ivec2 dir) const;

public:
	Area* area;
	int x, y, z;
	Exit* exits[5];
	boost::optional<double> layermods[5];
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
	TileType(TiledImage& img);

	//! Returns true if onscreen and we need to update our animation.
	bool needsRedraw() const;

public:
	Animation anim; //! Graphics for tiles of this type.
	std::vector<Tile*> allOfType;
};

void exportTile();

#endif

