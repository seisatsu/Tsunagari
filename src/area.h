/******************************
** Tsunagari Tile Engine     **
** area.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_H
#define AREA_H

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <Gosu/Audio.hpp> // for Gosu::SampleInstance
#include <Gosu/Graphics.hpp> // for Gosu::Transform
#include <libxml/tree.h>

#include "common.h"
#include "player.h"
#include "resourcer.h"

namespace Gosu {
	class Bitmap;
	class Button;
	class Image;
	class Sample;
	class SampleInstance;
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

	// when changing TileFlags, be sure to make updates to
	// Area::splitTileFlags()
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
	struct Tile;
	struct TileType {
		std::vector<ImageRef> graphics;
		std::vector<TileEvent> events;
		std::vector<Tile*> allOfType;
		Gosu::Image* graphic; // Current graphic to use for all tiles of
		                      // this type
		bool animated; // Is the tile animated?
		int frameLen; // Length of each frame in animation
		int animLen; // Total length of one complete cycle through
		             // animation
		int frameShowing; // Index of frame currently displaying on
		                  // screen
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


	//! Area Constructor
	Area(Resourcer* rc, World* world, Player* player,
			const std::string& filename);

	//! Area Destructor
	~Area();
	
	//! Function that must be called after the constructor.
	bool init();

	//! Gosu Callback
	void buttonDown(const Gosu::Button btn);
	void buttonUp(const Gosu::Button btn);

	//! Gosu Callback
	void draw();

	//! Gosu Callback
	bool needsRedraw() const;

	//! Gosu Callback
	void update(unsigned long dt);

	coord_t getDimensions() const;
	coord_t getTileDimensions() const;
	const Tile& getTile(coord_t c) const;
	Tile& getTile(coord_t c);

private:
	//! TileSet
	/*!
		Stores info for a tileset, and global settings for tiles.
	*/
	struct TileSet {
		TiledImage tiles;
		coord_t tileDim; // Dimensions per tile
		std::vector<TileType> tileTypes; // Global tile properties
	};

	//! Music
	/*!
		Stores info for the intro or main music files.
	*/
	struct Music {
		bool loop;
		std::string filename;
	};

	void drawTiles();
	void drawEntities();

	const coord_t viewportOffset() const;
	const Gosu::Transform viewportTransform() const;

	coordcube_t visibleTiles() const;
	bool tileTypeOnScreen(const TileType& type) const;

	//! XML descriptor parsing function.
	bool processDescriptor();

	//! XML descriptor parsing function.
	bool processMapProperties(xmlNode* node);

	//! XML descriptor parsing function.
	bool processTileSet(xmlNode* node);

	//! Constructs a tile of default type.
	TileType defaultTileType(TileSet& set);

	//! XML descriptor parsing function.
	bool processTileType(xmlNode* node, TileSet& ts);

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
	Door parseDoor(const std::string dest);


	Resourcer* rc;
	World* world;
	Player* player;
	const std::string descriptor;

	bool onIntro;
	SampleRef introMusic, mainMusic;
	boost::optional<Gosu::SampleInstance> musicInst;

	typedef std::vector<Tile> row_t;
	typedef std::vector<row_t> grid_t;
	typedef std::vector<grid_t> tilematrix_t;

	/* All layers in the map must be in the range of [0, n]. There cannot be
	 * any gaps.
	 */
	tilematrix_t map;
	coord_t dim;

	std::string name;
	std::string author;
	std::vector<TileSet> tilesets;
	std::string scripts;
	std::string onLoadEvents;
};

#endif

