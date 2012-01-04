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
	Area(Resourcer* rc, World* world, Viewport* view, Player* player,
	     Music* music, const std::string& filename);

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
	void requestRedraw();

	//! Gosu Callback
	void update(unsigned long dt);

	icoord getDimensions() const;
	ivec2 getTileDimensions() const;
	int depthIndex(double depth) const;
	double indexDepth(int idx) const;
	const Tile& getTile(icoord c) const;
	Tile& getTile(icoord c);
	bool tileExists(icoord c) const;
	icube_t visibleTiles() const;

	bool loopsInX() const;
	bool loopsInY() const;

	TileType& getTileType(int idx);
	void reset();

private:
	//! Calculate frame to show for each type of tile
	void updateTileAnimations();
	bool inBounds(int x, int y, int z) const;
	void drawTiles() const;
	void drawTile(const Tile& tile, int x, int y, double depth) const;
	void drawEntities();


	//! XML descriptor parsing function.
	bool processDescriptor();

	//! Allocate all Tile objects for one layer in 'dim' sized map.
	void allocateMapLayer();

	//! XML descriptor parsing function.
	bool processMapProperties(XMLNode node);

	//! XML descriptor parsing function.
	bool processTileSet(XMLNode node);

	//! XML descriptor parsing function.
	bool processTileType(XMLNode node, TiledImage& img, int id);

	//! XML descriptor parsing function.
	bool processLayer(XMLNode node);

	//! XML descriptor parsing function.
	bool processLayerProperties(XMLNode node, double* depth);

	//! XML descriptor parsing function.
	bool processLayerData(XMLNode node, int z);

	//! XML descriptor parsing function.
	bool processObjectGroup(XMLNode node);

	//! XML descriptor parsing function.
	bool processObjectGroupProperties(XMLNode node, double* depth);

	//! XML descriptor parsing function.
	bool processObject(XMLNode node, int z);

	//! Split a tile's flags into individuals.
	unsigned splitTileFlags(const std::string strOfFlags);

	//! Process a door convenience trigger.
	Door parseDoor(const std::string dest);


	Resourcer* rc;
	World* world;
	Viewport* view;
	Player* player;
	Music* music;
	bool redraw;
	const std::string descriptor;

	typedef std::vector<Tile> row_t;
	typedef std::vector<row_t> grid_t;
	typedef std::vector<grid_t> tilematrix_t;

	//! 3-dimensional array of the tiles that make up the map.
	tilematrix_t map;

	//! 3-dimensional length of map.
	icoord dim;

	//! Pixel size for each tile in area. All tiles in an Area must be the
	//! same size.
	ivec2 tileDim;

	//! Properties shared by all tiles of a type.
	std::vector<TileType> tileTypes;

	//! Maps virtual float-point depths to an index in our map array.
	std::map<double, int> depth2idx;

	//! Maps an index in our map array to a virtual float-point depth.
	std::vector<double> idx2depth;


	bool loopX, loopY;

	std::string name;
	std::string author;
	std::string scripts;
	std::string onLoadEvents;
};

void exportArea();

#endif

