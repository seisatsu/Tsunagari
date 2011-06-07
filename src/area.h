/******************************
** Tsunagari Tile Engine     **
** area.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_H
#define AREA_H

#include <string>
#include <vector>

#include <Gosu/Gosu.hpp>

#include "common.h"
#include "tile.h"

class Entity;
class GameWindow;
class Tile;
class TileMatrix;
class Resourcer;

//! Area Class
/*!
	This class is responsible for each map, or area in a Tsunagari World.
*/
class Area
{
public:
	//! Area Constructor
	Area(Resourcer* rc, Entity* player, const std::string filename);

	//! Area Destructor
	~Area();

	bool init();

	//! Gosu Callback
	void buttonDown(const Gosu::Button btn);

	//! Gosu Callback
	void draw();

	//! Gosu Callback
	bool needsRedraw() const;

	coord_t translateCoords(); //XXX What does this do?

private:
	bool processDescriptor();
	
	Resourcer* rc;
	Entity* player;
	TileMatrix* tiles;
	const std::string descriptor;
	
	//! AreaEvent XML Storage Struct
	/*!
		Stores info for an event attached to a tile.
	*/
	struct AreaEvent {
		Tile::TileEventTriggerTypes trigger;
		std::vector<std::string> argv; // Function name and arguments.
	};
	
	//! AreaMapTile XML Storage Struct
	/*!
		Stores a tile, including its animation properties, and things
		attached to it. This is later given to the Tile class
		constructor through TileMatrix.
	*/
	struct AreaMapTile {
		uint gid; // Tileset graphic position
		bool animated; // Is the tile animated?
		uint a_size; // Size of animation in tiles
		double a_speed; // Speed of animation in hertz
		std::vector<AreaEvent> events;
		std::vector<Tile::TileFlagTypes> flags;
	};
	
	//! AreaTileset XML Storage Struct
	/*!
		Stores info for a tileset, and global settings for tiles.
	*/
	struct AreaTileset {
		std::string filename;
		coord_t dim; // Dimensions of tileset image
		coord_t tiledim; // Dimensions per tile
		std::vector<AreaMapTile> globals; // Tiles with global settings
	};
	
	//! AreaMusic XML Storage Struct
	/*!
		Stores info for the intro or main music files.
	*/
	struct AreaMusic {
		bool loop;
		std::string filename;
	};
	
	//! AreaMapLayer XML Storage Struct
	/*!
		Stores layer level and layer's tile grid.
	*/
	struct AreaMapLayer {
		int level;
		std::vector<std::vector<AreaMapTile> > grid;
	};
	
	//! AreaValues XML Storage Struct
	/*!
		Main XML storage struct for Area.
	*/
	struct AreaValues {
		coord_t dim; // Dimensions of area
		std::string name;
		std::string author;
		std::vector<AreaTileset> tilesets;
		AreaMusic intro;
		AreaMusic main;
		std::vector<std::string> scripts;
		std::vector<std::vector<std::string> > onLoadEvents;
		std::vector<AreaMapLayer> map; // Given to TileMatrix.
	} xml;
};

#endif

