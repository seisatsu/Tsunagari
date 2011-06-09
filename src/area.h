/******************************
** Tsunagari Tile Engine     **
** area.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_H
#define AREA_H

#include <stack>
#include <string>
#include <vector>

#include <boost/scoped_ptr.hpp>
#include <Gosu/Gosu.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "common.h"
#include "tile.h"

class Entity;
class GameWindow;
class Tile;
class Resourcer;
class Sprite;
class Tile;

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

	coord_t getDimensions();
	Tile* getTile(coord_t c);

private:
	bool processDescriptor();
	
	//! Parser node stack
	std::stack<xmlNode*> xmlStack;
	
	Resourcer* rc;
	Entity* player;
	const std::string descriptor;

	typedef std::vector<Tile*> row_t;
	typedef std::vector<row_t> grid_t;
	typedef std::vector<grid_t> tilematrix_t;

	tilematrix_t matrix;
	coord_t dim;

	//! AreaEvent XML Storage Enum
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
//		boost::scoped_ptr<Sprite> tileset;
		std::vector<AreaTileset> tilesets;
		AreaMusic intro;
		AreaMusic main;
		std::vector<std::string> scripts;
		std::vector<std::vector<std::string> > onLoadEvents;
		std::vector<AreaMapLayer> map; // Given to TileMatrix.
	} xml;
};

#endif

