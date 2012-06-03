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
#include <boost/unordered_set.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/python/tuple.hpp>

#include <Gosu/Color.hpp>

#include "config.h"
#include "music.h"
#include "player.h"
#include "scriptinst.h"
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

//! An Area represents one map, or screen, in a Tsunagari World.
/*!
	The Area class handles the parsing of TMX-format Area descriptor files,
	the initialization, placement, and drawing of tiles, and various
	Area-related functionality.
*/
class Area
{
public:
	Area(Viewport* view, Player* player, Music* music,
	     const std::string& filename);
	virtual ~Area();

	//! Parse the file specified in the constructor, generating a full Area
	//! object. Must be called before use.
	virtual bool init();

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

	void setColorOverlay(int r, int g, int b, int a);

	const Tile* getTile(int x, int y, int z) const; /* phys */
	const Tile* getTile(int x, int y, double z) const; /* virt */
	const Tile* getTile(icoord phys) const;
	const Tile* getTile(vicoord virt) const;
	const Tile* getTile(rcoord virt) const;
	Tile* getTile(int x, int y, int z); /* phys */
	Tile* getTile(int x, int y, double z); /* virt */
	Tile* getTile(icoord phys);
	Tile* getTile(vicoord virt);
	Tile* getTile(rcoord virt);
	TileType& getGid(int idx);
	TileSet* getTileSet(std::string imagePath);

	//! Return the dimensions of the Tile matrix.
	ivec3 getDimensions() const;
	//! Return the pixel dimensions of a Tile graphic.
	ivec2 getTileDimensions() const;
	//! Returns a physical cubic range of Tiles that could be visible
	//! on-screen.
	icube visibleTileBounds() const;
	//! Returns a physical cubic range of Tiles that are visible on-screen.
	//! Takes actual map size into account.
	icube visibleTiles() const;

	//! Returns true if a Tile exists at the specified coordinate.
	bool inBounds(int x, int y, int z) const; /* phys */
	bool inBounds(int x, int y, double z) const; /* virt */
	bool inBounds(icoord phys) const;
	bool inBounds(vicoord virt) const;
	bool inBounds(rcoord virt) const;
	bool inBounds(Entity* ent) const;

	bool loopsInX() const;
	bool loopsInY() const;

	const std::string getDescriptor() const;

	Entity* spawnEntity(const std::string& descriptor,
		int x, int y, double z, const std::string& phase);
	void insert(Entity* e);
	void erase(Entity* e);

	// Convert between virtual and physical map coordinates. Physical
	// coordinates are the physical indexes into the Tile matrix. Layer
	// depth is represented by an arbirarily chosen integer in the physical
	// system. Virtual coordinates include the correct floating-point
	// depth.
	vicoord phys2virt_vi(icoord phys) const;
	rcoord phys2virt_r(icoord phys) const;
	icoord virt2phys(vicoord virt) const;
	icoord virt2phys(rcoord virt) const;
	rcoord virt2virt(vicoord virt) const;
	vicoord virt2virt(rcoord virt) const;

	// For Python interface.
	boost::python::tuple pyGetDimensions();


	//
	// Variables public for Python scripts
	//

	// Script hooks.
	ScriptInst loadScript, focusScript, updateScript;


protected:
	// Convert between virtual and physical map depths.
	int depthIndex(double depth) const;
	double indexDepth(int idx) const;

	//! Run scripts that needs to be run before this Area is usable.
	void runLoadScripts();

	//! Calculate frame to show for each type of tile
	void updateTileAnimations();
	void drawTiles() const;
	void drawTile(const Tile& tile, int x, int y, double depth) const;
	void drawEntities();
	void drawColorOverlay();

protected:
	Viewport* view;
	Player* player;
	boost::unordered_set<Entity*> entities;
	Music* music;
	Gosu::Color colorOverlay;

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

	typedef std::map<std::string, TileSet> tilesets_t;
	tilesets_t tileSets;

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
};

//! Register Areas with Python.
void exportArea();

#endif

