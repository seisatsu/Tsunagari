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
struct Door;
class Entity;
class Tile;
struct TileEvent;
class TileType;

//! List of possible triggers for tile events.
/*!
	Triggers describe the conditions for the activation of a
	tile-bound event script funtion.
*/
enum TileEventTrigger {
	onEnter,
	onLeave,
	onUse,
	door
};

//! List of possible flags that can be attached to a tile.
/*!
	Flags are attached to tiles, and denote special behavior for
	the tile they are bound to.
*/
enum TileFlags {
	// When changing TileFlags, be sure to make updates to
	// Area::splitTileFlags().
	nowalk        = 0x0001,
	player_nowalk = 0x0002,
	npc_nowalk    = 0x0004,
	player_event  = 0x0008,
	npc_event     = 0x0010,
	temp_event    = 0x0020,

	// Event flags, don't parse for these. We set them ourselves.
	hasOnEnter    = 0x0100,
	hasOnLeave    = 0x0200,
	hasOnUse      = 0x0400
};

//! Stores info for an event attached to a tile.
/*!
	Events are attached to tiles, and parsed into this struct from a
	TMX-format area descriptor file. The event is executed when the
	condition for its trigger is met. The event function name and
	the function's arguments are stored in argv.
*/
struct TileEvent {
	TileEventTrigger trigger;
	std::string script; // Filename.
};

//! Convenience trigger for inter-area teleportation.
/*!
	Tiles with a door trigger attached can teleport the player to a
	new area in the World. The Door struct contains the destination
	area and coordinates.
*/
struct Door {
	Door();
	Door(const std::string area, int x, int y, double z);

	std::string area;
	vicoord tile;
};

//! Contains properties unique to this tile.
/*!
	This struct contains local tile properties for a single tile in
	the area. As opposed to global properties which apply to all
	tiles of the same type, these properties will only apply to one
	tile.
*/
class Tile
{
public:
	Tile();

	TileType* type;
	std::vector<TileEvent> events;
	boost::optional<Door> door;
	boost::optional<double> layermod;
	unsigned flags; //! Flags for each option in TileFlags enum.

	//! Determines whether this tile or one of its parent types embodies a
	//! flag.
	bool hasFlag(unsigned flag) const;

	void onEnterScripts(Entity* triggeredBy);
	void onLeaveScripts(Entity* triggeredBy);
	void onUseScripts(Entity* triggeredBy);

	boost::optional<Door> getDoor();
	void setDoor(boost::optional<Door> d);

	void setWalkable(bool b);
	bool getWalkable();

private:
	//! Runs all scripts owned by this tile or its type.
	void runScripts(Entity* entity,
	                TileFlags flag, TileEventTrigger trigger);

	//! Runs all scripts from a group that match the trigger.
	void runScriptGroup(Entity* entity,
	                    TileEventTrigger trigger,
	                    const std::vector<TileEvent>& events);

	//! Runs a single script.
	void runScript(Entity* entity,
	               const std::string& script);
};

//! Contains the properties shared by all tiles of a certain type.
/*!
	This struct contains global tile properties for a tile of a
	certain type. As opposed to local properties for a single tile,
	all tiles of this type will share the defined characteristics.
*/
class TileType
{
public:
	TileType();
	TileType(TiledImage& img);

	//! Returns true if onscreen and we need to update our animation.
	bool needsRedraw(const Area& area) const;

	Animation anim; //! Graphics for tiles of this type.
	std::vector<TileEvent> events;
	std::vector<Tile*> allOfType;
	boost::optional<double> layermod;
	unsigned flags; //! Flags for each option in TileFlags enum.

private:
	//! Returns true if any of the area's tiles within the specified range
	//! are of this type.
	bool visibleIn(const Area& area, const icube_t& tiles) const;
};

void exportTile();
void exportTileType();
void exportDoor();

#endif

