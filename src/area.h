/******************************
** Tsunagari Tile Engine     **
** area.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_H
#define AREA_H

#include <string>
#include <vector>

#include <boost/scoped_ptr.hpp>
#include <Gosu/Audio.hpp> // for Gosu::SampleInstance
#include <Gosu/Graphics.hpp> // for Gosu::Transform
#include <libxml/tree.h>

#include "area-structs.h"
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

	icoord_t getDimensions() const;
	icoord_t getTileDimensions() const;
	const Tile& getTile(icoord_t c) const;
	Tile& getTile(icoord_t c);

private:
	//! TileSet
	/*!
		Stores info for a tileset, and global settings for tiles.
	*/
	struct TileSet {
		TiledImage tiles;
		icoord_t tileDim; // Dimensions per tile
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

	//! Calculate frame to show for each type of tile
	void updateTileAnimations();
	void drawTiles() const;
	void drawTile(const Tile& tile, int x, int y, int z) const;
	void drawEntities();

	const icoord_t viewportOffset() const;
	const Gosu::Transform viewportTransform() const;

	icube_t visibleTiles() const;
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
	icoord_t dim;

	std::string name;
	std::string author;
	std::vector<TileSet> tilesets;
	std::string scripts;
	std::string onLoadEvents;
};

#endif

