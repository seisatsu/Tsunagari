/******************************
** Tsunagari Tile Engine     **
** area-structs.h            **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_STRUCTS_H
#define AREA_STRUCTS_H

#include <string>

#include <boost/optional.hpp>

#include "animation.h"
#include "common.h"

//! List of possible triggers for tile events.
/*!
	Triggers describe the conditions for the activation of a 
	tile-bound event script funtion.
*/
enum TileEventTriggers {
	onUse,
	onEnter,
	onLeave,
	door
};

//! List of possible flags that can be attached to a tile.
/*!
	Flags are attached to tiles, and denote special behavior for 
	the tile they are bound to.
*/
enum TileFlags {
	// when changing TileFlags, be sure to make updates to
	// Area::splitTileFlags()
	nowalk        = 0x0001,
	player_nowalk = 0x0002,
	npc_nowalk    = 0x0004,
	player_event  = 0x0008,
	npc_event     = 0x0010,
	temp_event    = 0x0020,

	// Event flags, don't parse for these. We set them ourselves.
	hasOnEnter    = 0x0100,
	hasOnLeave    = 0x0200
};

//! Stores info for an event attached to a tile.
/*!
	Events are attached to tiles, and parsed into this struct from a
	TMX-format area descriptor file. The event is executed when the 
	condition for its trigger is met. The event function name and 
	the function's arguments are stored in argv.
*/
struct TileEvent {
	TileEventTriggers trigger;
	std::string script; // Script file.
};

//! Convenience trigger for inter-area teleportation.
/*!
	Tiles with a door trigger attached can teleport the player to a 
	new area in the World. The Door struct contains the destination 
	area and coordinates.
*/
struct Door {
	std::string area;
	icoord_t tile;
};

//! Contains the properties shared by all tiles of a certain type.
/*!
	This struct contains global tile properties for a tile of a 
	certain type. As opposed to local properties for a single tile, 
	all tiles of this type will share the defined characteristics.
*/
struct Tile;
struct TileType {
	Animation anim; // Graphics for tiles of this type.
	std::vector<TileEvent> events;
	std::vector<Tile*> allOfType;
	unsigned flags; // bitflags for each option in TileFlags enum
	// TODO: boost::scoped_ptr<Door> door
};

//! Contains properties unique to this tile.
/*!
	This struct contains local tile properties for a single tile in 
	the area. As opposed to global properties which apply to all 
	tiles of the same type, these properties will only apply to one 
	tile.
*/
struct Tile {
	TileType* type;
	std::vector<TileEvent> events;
	unsigned flags; // bitflags for each option in TileFlags enum
	boost::optional<Door> door;
};

#endif

