/******************************
** Tsunagari Tile Engine     **
** area.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_H
#define AREA_H

#include <string>
#include <vector>

#include <Gosu/Bitmap.hpp>
#include <Gosu/Graphics.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "common.h"

namespace Gosu {
	class Button;
	class Image;
}

class Entity;
class GameWindow;
class Resourcer;
class Sprite;
class World;

//! Area Class
/*!
	This class is responsible for each map, or area in a Tsunagari World.
*/
class Area
{
public:

	// when changing TileFlags, be sure to make updates to Area::splitTileFlags()
	enum TileFlags {
		nowalk        = 0x0001,
		player_nowalk = 0x0002,
		npc_nowalk    = 0x0004,
		player_event  = 0x0008,
		npc_event     = 0x0010,
		temp_event    = 0x0020
	};
	
	enum TileEventTriggers {
		onUse,
		onEnter,
		onLeave,
		door
	};

	//! TileEvent
	/*!
		Stores info for an event attached to a tile.
	*/
	struct TileEvent {
		TileEventTriggers trigger;
		std::string argv; // Function name and arguments.
	};

	//! Door
	/*!
		Some tiles can teleport you to a new area or a new position
		in your current area.
	*/
	struct Door {
		std::string area;
		coord_t coord;
	};
	
	//! TileType
	/*!
		Contains the properties shared by all tiles of a certain type.
		E.g.: all grass tiles have the same graphic, and all wall tiles
		      are unwalkable.
	*/
	struct TileType {
		std::vector<Gosu::Image*> graphics;
		bool animated; // Is the tile animated?
		double ani_speed; // Speed of animation in hertz
		std::vector<TileEvent> events;
		unsigned flags; // bitflags for each option in TileFlags enum
		// TODO: Door* door
	};

	//! Tile
	/*!
		Stores a tile, including its animation properties, and things
		attached to it. This is later given to the Tile class
		constructor through TileMatrix.
	*/
	struct Tile {
		TileType* type;
		std::vector<TileEvent> events;
		unsigned flags; // bitflags for each option in TileFlags enum
		Door* door;
	};


	//! Area Constructor
	Area(Resourcer* rc, World* world, Entity* player, const std::string& filename);

	//! Area Destructor
	~Area();

	bool init();

	//! Gosu Callback
	void buttonDown(const Gosu::Button btn);

	//! Gosu Callback
	void draw();

	//! Gosu Callback
	bool needsRedraw() const;

	coord_t getDimensions() const;
	Tile* getTile(coord_t c);

private:
	//! Tileset
	/*!
		Stores info for a tileset, and global settings for tiles.
	*/
	struct Tileset {
		Gosu::Bitmap* source; // TODO: make shared_ptr for memory sureness, also delete after done loading tileset
		coord_t tiledim; // Dimensions per tile
		std::vector<TileType> defaults; // Global tile properties
	};

	//! Music
	/*!
		Stores info for the intro or main music files.
	*/
	struct Music {
		bool loop;
		std::string filename;
	};

	bool processDescriptor();
	bool processMapProperties(xmlNode* node);
	TileType defaultTileType(const Gosu::Bitmap* source, coord_t tiledim,
	                         int id);
	bool processTileset(xmlNode* node);
	bool processTileType(xmlNode* node, Tileset& ts);
	bool processLayer(xmlNode* node);
	bool processLayerProperties(xmlNode* node);
	bool processLayerData(xmlNode* node);
	bool processObjectGroup(xmlNode* node);
	bool processObjectGroupProperties(xmlNode* node, int* zpos);
	bool processObject(xmlNode* node, int zpos);
	unsigned splitTileFlags(const std::string strOfFlags);
	Door* parseDoor(const std::string dest);

	Gosu::Transform translateCoords();


	Resourcer* rc;
	World* world;
	Entity* player;
	const std::string descriptor;

	typedef std::vector<Tile*> row_t;
	typedef std::vector<row_t> grid_t;
	typedef std::vector<grid_t> tilematrix_t;

	/* All layers in the map must be in the range of [0, n]. There cannot be
	 * any gaps.
	 */
	tilematrix_t map;
	coord_t dim;
	
	std::string name;
	std::string author;
	std::vector<Tileset> tilesets;
	Music intro;
	Music main;
	std::string scripts;
	std::string onLoadEvents;
};

#endif

