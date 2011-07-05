/******************************
** Tsunagari Tile Engine     **
** area.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_H
#define AREA_H

#include <string>
#include <vector>

#include <Gosu/Audio.hpp>
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

//! This class is responsible for each map, or area in a Tsunagari World.
/*!
	The Area class handles the parsing of TMX-format Area descriptor files, 
	the initialization, placement, and drawing of tiles, and various 
	Area-related functionality.
*/
class Area
{
public:

	// when changing TileFlags, be sure to make updates to Area::splitTileFlags()
	//! List of possible flags that can be attached to a tile.
	/*!
		Flags are attached to tiles, and denote special behavior for 
		the tile they are bound to.
	*/
	enum TileFlags {
		nowalk        = 0x0001,
		player_nowalk = 0x0002,
		npc_nowalk    = 0x0004,
		player_event  = 0x0008,
		npc_event     = 0x0010,
		temp_event    = 0x0020
	};
	
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

	//! Stores info for an event attached to a tile.
	/*!
		Events are attached to tiles, and parsed into this struct from a
		TMX-format area descriptor file. The event is executed when the 
		condition for its trigger is met. The event function name and 
		the function's arguments are stored in argv.
	*/
	struct TileEvent {
		TileEventTriggers trigger;
		std::string argv; // Function name and arguments.
	};

	//! Convenience trigger for inter-area teleportation.
	/*!
		Tiles with a door trigger attached can teleport the player to a 
		new area in the World. The Door struct contains the destination 
		area and coordinates.
	*/
	struct Door {
		std::string area;
		coord_t coord;
	};
	
	//! Contains the properties shared by all tiles of a certain type.
	/*!
		This struct contains global tile properties for a tile of a 
		certain type. As opposed to local properties for a single tile, 
		all tiles of this type will share the defined characteristics.
	*/
	struct TileType {
		std::vector<Gosu::Image*> graphics;
		bool animated; // Is the tile animated?
		double ani_speed; // Speed of animation in hertz
		std::vector<TileEvent> events;
		unsigned flags; // bitflags for each option in TileFlags enum
		// TODO: Door* door
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
		Door* door;
	};


	//! Area Constructor
	Area(Resourcer* rc, World* world, Entity* player, const std::string& filename);

	//! Area Destructor
	~Area();
	
	//! Function that must be called after the constructor.
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
		Gosu::Bitmap source;
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

	//! XML descriptor parsing function.
	bool processDescriptor();
	
	//! XML descriptor parsing function.
	bool processMapProperties(xmlNode* node);
	
	//! Constructs a tile of default type.
	TileType defaultTileType(const Gosu::Bitmap source, coord_t tiledim,
	                         int id);
	
	//! XML descriptor parsing function.
	bool processTileset(xmlNode* node);
	
	//! XML descriptor parsing function.
	bool processTileType(xmlNode* node, Tileset& ts);
	
	//! XML descriptor parsing function.
	bool processLayer(xmlNode* node);
	
	//! XML descriptor parsing function.
	bool processLayerProperties(xmlNode* node);
	
	//! XML descriptor parsing function.
	bool processLayerData(xmlNode* node);
	
	//! XML descriptor parsing function.
	bool processObjectGroup(xmlNode* node);
	
	//! XML descriptor parsing function.
	bool processObjectGroupProperties(xmlNode* node, int* zpos);
	
	//! XML descriptor parsing function.
	bool processObject(xmlNode* node, int zpos);
	
	//! Split a tile's flags into individuals.
	unsigned splitTileFlags(const std::string strOfFlags);
	
	//! Process a door convenience trigger.
	Door* parseDoor(const std::string dest);
	
	Gosu::Transform translateCoords();


	Resourcer* rc;
	World* world;
	Entity* player;
	const std::string descriptor;
	// Disabled, waiting on Gosu change
	//Gosu::Sample* music_buf;
	//Gosu::SampleInstance* music_inst;

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
	Music music_intro;
	Music music_main;
	std::string scripts;
	std::string onLoadEvents;
};

#endif

