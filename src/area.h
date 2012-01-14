/*********************************
** Tsunagari Tile Engine        **
** area.h                       **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef AREA_H
#define AREA_H

#include <map>
#include <string>
#include <vector>

#include <boost/unordered_map.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "common.h"
#include "config.h"
#include "music.h"
#include "player.h"
#include "resourcer.h"
#include "tile.h"
#include "viewport.h"
#include "xml.h"

namespace Gosu {
	class Bitmap;
	class Button;
	class Image;
}

class Area;
class Entity;
class GameWindow;
class Music;
class Resourcer;
class Sprite;
class World;

typedef boost::shared_ptr<Area> AreaPtr;

/**
 * Virtual tile coordinate.
 *
 * x and y are the same as a physical coordinate.
 * z is the layer depth.
 */
struct vtcoord
{
	vtcoord(int x, int y, double z): x(x), y(y), z(z) {}

	int x, y;
	double z;
};

//! An Area represents one map, or screen, in a Tsunagari World.
/*!
	The Area class handles the parsing of TMX-format Area descriptor files,
	the initialization, placement, and drawing of tiles, and various
	Area-related functionality.
*/
class Area
{
public:
	Area(Resourcer* rc, World* world, Viewport* view, Player* player,
	     Music* music, const std::string& filename);
	~Area();

	//! Parse the file specified in the constructor, generating a full Area
	//! object. Must be called before use.
	bool init();

	//! Prepare game state for this Area to be in focus.
	void focus();

	//! Processes keyboard input, calling the Player object when necessary.
	void buttonDown(const Gosu::Button btn);
	void buttonUp(const Gosu::Button btn);

	//! Renders all visible Tiles and Entities within this Area.
	void draw();

	//! If false, drawing might be skipped. Saves CPU cycles when idle.
	bool needsRedraw() const;

	//! Inform the Area that a redraw is needed.
	void requestRedraw();

	//! Update the game state within this Area as if dt milliseconds had
	//! passed since the last call. Updates Entities and runs scripts.
	void update(unsigned long dt);

	//! Creates a new Area based off the same descriptor file and focuses
	//! it. The Player's location is preserved.
	void reset();

	bool tileExists(icoord phys) const;
	icube_t visibleTiles() const;
	const Tile& getTile(icoord phys) const;
	const Tile& getTile(vtcoord virt) const;
	Tile& getTile(icoord phys);
	Tile& getTile(vtcoord virt);
	TileType& getTileType(int idx);

	//! Return the dimensions of the Tile matrix.
	ivec3 getDimensions() const;
	//! Return the pixel dimensions of a Tile graphic.
	ivec2 getTileDimensions() const;

	bool loopsInX() const;
	bool loopsInY() const;

	const std::string& getDescriptor() const;


	// Convert between virtual and physical map coordinates. Physical
	// coordinates are the physical indexes into the Tile matrix. Layer
	// depth is represented by an arbirarily chosen integer in the physical
	// system. Virtual coordinates include the correct floating-point
	// depth.
	// {
	vtcoord phys2virt(icoord phys) /* const */;
	rcoord phys2virt(icoord phys) const;
	icoord virt2phys(vtcoord virt) const;
	icoord virt2phys(rcoord virt) const;
	int depthIndex(double depth) const;
	double indexDepth(int idx) const;
	// }

private:
	//! Run all scripts that need to be run before this Area is usable.
	void runOnLoads();

	//! Calculate frame to show for each type of tile
	void updateTileAnimations();
	bool inBounds(int x, int y, int z) const;
	void drawTiles() const;
	void drawTile(const Tile& tile, int x, int y, double depth) const;
	void drawEntities();


	//! Allocate Tile objects for one layer of map.
	void allocateMapLayer();

	//! Parse an Area file.
	bool processDescriptor();
	bool processMapProperties(XMLNode node);
	bool processTileSet(XMLNode node);
	bool processTileType(XMLNode node, TiledImage& img, int id);
	bool processLayer(XMLNode node);
	bool processLayerProperties(XMLNode node, double* depth);
	bool processLayerData(XMLNode node, int z);
	bool processObjectGroup(XMLNode node);
	bool processObjectGroupProperties(XMLNode node, double* depth);
	bool processObject(XMLNode node, int z);
	unsigned splitTileFlags(const std::string& strOfFlags);
	Door parseDoor(const std::string& dest);

private:
	Resourcer* rc;
	World* world;
	Viewport* view;
	Player* player;
	Music* music;


	typedef std::vector<Tile> row_t;
	typedef std::vector<row_t> grid_t;
	typedef std::vector<grid_t> tilematrix_t;

	//! 3-dimensional array of the tiles that make up the map.
	tilematrix_t map;

	//! 3-dimensional length of map.
	ivec3 dim;

	//! Pixel size for each tile in area. All tiles in an Area must be the
	//! same size.
	ivec2 tileDim;

	//! Properties shared by all tiles of a type.
	std::vector<TileType> tileTypes;

	//! Maps virtual float-point depths to an index in our map array.
	boost::unordered_map<double, int> depth2idx;

	//! Maps an index in our map array to a virtual float-point depth.
	std::vector<double> idx2depth;


	std::string name, author;
	bool loopX, loopY;
	bool beenFocused;
	bool redraw;

	// The following contain filenames such that they may be loaded lazily.
	const std::string descriptor;
	std::string musicIntro, musicLoop;
	std::vector<std::string> onLoadScripts;
	std::vector<std::string> onUpdateScripts;
};

//! Register Areas with Python.
void exportArea();

#endif

