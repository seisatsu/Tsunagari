/******************************
** Tsunagari Tile Engine     **
** area.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>

#include "area.h"
#include "common.h"
#include "entity.h"
#include "log.h"
#include "resourcer.h"
#include "sprite.h"
#include "window.h"
#include "world.h"

/* NOTE: Tileset tiles start counting their positions from 0, while layer tiles
         start counting from 1. I can't imagine why the author did this, but we
         have to take it into account. 
*/

Area::Area(Resourcer* rc,
           World* world,
           Entity* player,
           const std::string& descriptor)
	: rc(rc), world(world), player(player), descriptor(descriptor)
{
	dim.x = dim.y = dim.z = 0;
}

Area::~Area()
{
	// Delete each Tile. If a Tile has an allocated Door struct, delete
	// that as well.
	BOOST_FOREACH(grid_t grid, map) {
		BOOST_FOREACH(row_t row, grid) {
			BOOST_FOREACH(Tile* tile, row) {
				delete tile->door;
				delete tile;
			}
		}
	}

	// Each Area owns its own Tileset objects. Delete tileset graphics.
	BOOST_FOREACH(Tileset tileset, tilesets) {
		BOOST_FOREACH(TileType type, tileset.defaults) {
			BOOST_FOREACH(Gosu::Image* img, type.graphics) {
				delete img;
			}
			// TODO: delete TileEvents when we start using them
		}
	}
}

bool Area::init()
{
	return processDescriptor();
}

void Area::buttonDown(const Gosu::Button btn)
{
	bool attemptingMove = false;
	coord_t posMove;

	if (btn == Gosu::kbRight) {
		posMove = coord(1, 0, 0);
		attemptingMove = true;
	}
	else if (btn == Gosu::kbLeft) {
		posMove = coord(-1, 0, 0);
		attemptingMove = true;
	}
	else if (btn == Gosu::kbUp) {
		posMove = coord(0, -1, 0);
		attemptingMove = true;
	}
	else if (btn == Gosu::kbDown) {
		posMove = coord(0, 1, 0);
		attemptingMove = true;
	}

	if (attemptingMove) {
		coord_t newCoord = player->getCoordsByTile();
		newCoord.x += posMove.x;
		newCoord.y += posMove.y;
		newCoord.z += posMove.z;
		Tile* dest = getTile(newCoord);
		if ((dest->flags       & player_nowalk) != 0 ||
		    (dest->type->flags & player_nowalk) != 0) {
			// The tile we're trying to move onto is set as nowalk
			// for the player. Stop here.
			return;
		}
		if (dest->door) {
			world->loadArea(dest->door->area, dest->door->coord);
		}
		else {
			player->moveByTile(posMove);
		}
	}
}

void Area::draw()
{
	GameWindow* window = GameWindow::getWindow();
	Gosu::Graphics* graphics = &window->graphics();
	Gosu::Transform trans = translateCoords();
	graphics->pushTransform(trans);

	for (unsigned int layer = 0; layer != map.size(); layer++)
	{
		grid_t grid = map[layer];
		for (unsigned int y = 0; y != grid.size(); y++)
		{
			row_t row = grid[y];
			for (unsigned int x = 0; x != row.size(); x++)
			{
				// TODO support animations
				Tile* tile = row[x];
				Gosu::Image* img = tile->type->graphics[0];
				img->draw(x*img->width(), y*img->height(), 0);
			}
		}
	}
	player->draw();

	graphics->popTransform();
}

//! Returns the number closest to x within the range [low, high].
/*!
	\param low Lowest possible return.
	\param x Value to be bounded.
	\param high Highest possible return.
	\return A number close to x.
	\sa center() and translateCoords()
*/
static double bound(double low, double x, double high)
{
	if (low > x)
		x = low;
	if (x > high)
		x = high;
	return x;
}

static double center(double w, double g, double p)
{
	return w>g ? (w-g)/2.0 : bound(w-g, w/2.0-p, 0);
}

Gosu::Transform Area::translateCoords()
{
	GameWindow* window = GameWindow::getWindow();
	Gosu::Graphics* graphics = &window->graphics();

	double tileWidth = (double)tilesets[0].tiledim.x;
	double tileHeight = (double)tilesets[0].tiledim.y;
	double windowWidth = (double)graphics->width() / tileWidth;
	double windowHeight = (double)graphics->height() / tileHeight;
	double gridWidth = (double)dim.x;
	double gridHeight = (double)dim.y;
	double playerX = (double)player->getCoordsByPixel().x / tileWidth + 0.5;
	double playerY = (double)player->getCoordsByPixel().y / tileHeight + 0.5;

	coord_t c;
	c.x = (long)center(windowWidth, gridWidth, playerX) * (long)tileWidth;
	c.y = (long)center(windowHeight, gridHeight, playerY) * (long)tileHeight;

	Gosu::Transform trans = Gosu::translate((double)c.x, (double)c.y);
	return trans;
}

bool Area::needsRedraw() const
{
	return player->needsRedraw();
}

bool Area::processDescriptor()
{
	xmlDoc* doc = rc->getXMLDoc(descriptor);
	if (!doc)
		return false;

	// use RAII to ensure doc is freed
	boost::shared_ptr<void> alwaysFreeTheDoc(doc, xmlFreeDoc);

	// Iterate and process children of <map>
	xmlNode* root = xmlDocGetRootElement(doc); // <map> element

	xmlChar* width = xmlGetProp(root, BAD_CAST("width"));
	xmlChar* height = xmlGetProp(root, BAD_CAST("height"));
	dim.x = atol((const char*)width);
	dim.y = atol((const char*)height);

	xmlNode* child = root->xmlChildrenNode;
	for (; child != NULL; child = child->next) {
		if (!xmlStrncmp(child->name, BAD_CAST("properties"), 11)) {
			if (!processMapProperties(child))
				return false;
		}
		else if (!xmlStrncmp(child->name, BAD_CAST("tileset"), 8)) {
			if (!processTileset(child))
				return false;
		}
		else if (!xmlStrncmp(child->name, BAD_CAST("layer"), 6)) {
			if (!processLayer(child))
				return false;
		}
		else if (!xmlStrncmp(child->name, BAD_CAST("objectgroup"), 12)) {
			if (!processObjectGroup(child))
				return false;
		}
	}

	return true;
}

bool Area::processMapProperties(xmlNode* node)
{

/*
 <properties>
  <property name="areaspec" value="1"/>
  <property name="author" value="Michael D. Reiley"/>
  <property name="name" value="Baby's First Area"/>
  <property name="music_loop" value="true"/>
  <property name="music_main" value="wind.music"/>
  <property name="onLoad" value="babysfirst_init()"/>
  <property name="scripts" value="areainits.event,test.event"/>
 </properties>
*/

	xmlNode* child = node->xmlChildrenNode;
	for (; child != NULL; child = child->next) {
		xmlChar* name = xmlGetProp(child, BAD_CAST("name"));
		xmlChar* value = xmlGetProp(child, BAD_CAST("value"));
		if (!xmlStrncmp(name, BAD_CAST("author"), 7))
			author = (const char*)value;
		else if (!xmlStrncmp(name, BAD_CAST("name"), 5))
			this->name = (const char*)value;
		else if (!xmlStrncmp(name, BAD_CAST("music_loop"), 11))
			main.loop = parseBool((const char*)value);
		else if (!xmlStrncmp(name, BAD_CAST("music_main"), 11))
			main.filename = (const char*)value;
		else if (!xmlStrncmp(name, BAD_CAST("onLoad"), 7))
			onLoadEvents = (const char*)value;
		else if (!xmlStrncmp(name, BAD_CAST("scripts"), 8))
			scripts = (const char*)value; // TODO split(), load
	}
	return true;
}

bool Area::processTileset(xmlNode* node)
{

/*	
 <tileset firstgid="1" name="tiles.sheet" tilewidth="64" tileheight="64">
  <image source="tiles.sheet" width="256" height="256"/>
  <tile id="14">
   ...
  </tile>
 </tileset>
*/
	Tileset ts;
	xmlChar* width = xmlGetProp(node, BAD_CAST("tilewidth"));
	xmlChar* height = xmlGetProp(node, BAD_CAST("tileheight"));
	ts.tiledim.x = atol((const char*)width);
	ts.tiledim.y = atol((const char*)height);
	
	xmlNode* child = node->xmlChildrenNode;
	for (; child != NULL; child = child->next) {
		if (!xmlStrncmp(child->name, BAD_CAST("tile"), 5)) {
			xmlChar* idstr = xmlGetProp(child, BAD_CAST("id"));
			unsigned id = (unsigned)atoi((const char*)idstr);

			// Undeclared TileTypes have default properties.
			while (ts.defaults.size() != id) {
				TileType tt = defaultTileType(ts.source,
					ts.tiledim, (int)ts.defaults.size());
				ts.defaults.push_back(tt);
			}

			// Handle explicit TileType
			if (!processTileType(child, ts))
				return false;
		}
		else if (!xmlStrncmp(child->name, BAD_CAST("image"), 6)) {
			xmlChar* source = xmlGetProp(child, BAD_CAST("source"));
			ts.source = rc->getBitmap((const char*)source);
		}
	}

	// Generate default tile types in range (m,n] where m is the last
	// explicitly declared type and n is the number we require.
	unsigned srcsz = ts.source.width() * ts.source.height();
	unsigned long tilesz = (unsigned long)(ts.tiledim.x * ts.tiledim.y);
	while (ts.defaults.size() != srcsz / tilesz) {
		TileType tt = defaultTileType(ts.source,
			ts.tiledim, (int)ts.defaults.size());
		ts.defaults.push_back(tt);
	}

	tilesets.push_back(ts);
	return true;
}

Area::TileType Area::defaultTileType(const Gosu::Bitmap source, coord_t tiledim,
                               int id)
{
	unsigned x = (unsigned)((tiledim.x * id) % source.width());
	unsigned y = (unsigned)((tiledim.y * id) / source.height() * tiledim.y); // ???
	
	TileType tt;
	Gosu::Image* img = rc->bitmapSection(source, x, y,
	                                     (unsigned)tiledim.x, (unsigned)tiledim.y, true);
	tt.graphics.push_back(img);
	tt.animated = false;
	tt.ani_speed = 0.0;
	tt.flags = 0x0;
	return tt;
}

bool Area::processTileType(xmlNode* node, Tileset& ts)
{

/*
  <tile id="8">
   <properties>
    <property name="flags" value="nowalk"/>
    <property name="onEnter" value="skid();speed(2)"/>
    <property name="onLeave" value="undo()"/>
   </properties>
  </tile>
  <tile id="14">
   <properties>
    <property name="animated" value="1"/>
    <property name="size" value="2"/>
    <property name="speed" value="2"/>
   </properties>
  </tile>
*/

	// Initialize a default TileType, we'll build on that.
	TileType tt = defaultTileType(ts.source,
		ts.tiledim, (int)ts.defaults.size());

	xmlChar* idstr = xmlGetProp(node, BAD_CAST("id"));
	unsigned id = (unsigned)atoi((const char*)idstr);
	if (id != ts.defaults.size()) {
		// XXX we need to know the Area we're loading...
		Log::err("unknown area", std::string("expected TileType id ") +
		         itostr((long)ts.defaults.size()) + ", but got " + itostr(id));
		return false;
	}

	xmlNode* child = node->xmlChildrenNode; // <properties>
	child = child->xmlChildrenNode; // <property>
	for (; child != NULL; child = child->next) {
		xmlChar* name = xmlGetProp(child, BAD_CAST("name"));
		xmlChar* value = xmlGetProp(child, BAD_CAST("value"));
		if (!xmlStrncmp(name, BAD_CAST("flags"), 6)) {
			tt.flags = splitTileFlags((const char*)value);
		}
		else if (!xmlStrncmp(name, BAD_CAST("onEnter"), 8)) {
			// TODO events
		}
		else if (!xmlStrncmp(name, BAD_CAST("onLeave"), 8)) {
			// TODO events
		}
		else if (!xmlStrncmp(name, BAD_CAST("animated"), 9)) {
			tt.animated = parseBool((const char*)value);
		}
		else if (!xmlStrncmp(name, BAD_CAST("size"), 5)) {
			// TODO animation
		}
		else if (!xmlStrncmp(name, BAD_CAST("speed"), 6)) {
			tt.ani_speed = atof((const char*)value);
		}
	}

	ts.defaults.push_back(tt);
	return true;
}

bool Area::processLayer(xmlNode* node)
{

/*
 <layer name="Tiles0" width="5" height="5">
  <properties>
   ...
  </properties>
  <data>
   <tile gid="9"/>
   <tile gid="9"/>
   <tile gid="9"/>
...
   <tile gid="3"/>
   <tile gid="9"/>
   <tile gid="9"/>
  </data>
 </layer>
*/

	xmlChar* width = xmlGetProp(node, BAD_CAST("width"));
	xmlChar* height = xmlGetProp(node, BAD_CAST("height"));
	int x = atoi((const char*)width);
	int y = atoi((const char*)height);

	if (dim.x != x || dim.y != y) {
		// XXX we need to know the Area we're loading...
		Log::err("unknown area", "layer x,y size != map x,y size");
		return false;
	}

	xmlNode* child = node->xmlChildrenNode;
	for (; child != NULL; child = child->next) {
		if (!xmlStrncmp(child->name, BAD_CAST("properties"), 11)) {
			if (!processLayerProperties(child))
				return false;
		}
		else if (!xmlStrncmp(child->name, BAD_CAST("data"), 5)) {
			if (!processLayerData(child))
				return false;
		}
	}
	return true;
}

bool Area::processLayerProperties(xmlNode* node)
{

/*
  <properties>
   <property name="layer" value="0"/>
  </properties>
*/

	xmlNode* child = node->xmlChildrenNode;
	for (; child != NULL; child = child->next) {
		xmlChar* name = xmlGetProp(child, BAD_CAST("name"));
		xmlChar* value = xmlGetProp(child, BAD_CAST("value"));
		if (!xmlStrncmp(name, BAD_CAST("layer"), 6)) {
			int depth = atoi((const char*)value);
			if (depth != dim.z) {
				Log::err("unknown area", "invalid layer depth");
				return false;
			}
		}
	}

	return true;
}

bool Area::processLayerData(xmlNode* node)
{

/*
  <data>
   <tile gid="9"/>
   <tile gid="9"/>
   <tile gid="9"/>
...
   <tile gid="3"/>
   <tile gid="9"/>
   <tile gid="9"/>
  </data>
*/

	row_t row;
	grid_t grid;

	xmlNode* child = node->xmlChildrenNode;
	for (int i = 1; child != NULL; i++, child = child->next) {
		if (!xmlStrncmp(child->name, BAD_CAST("tile"), 5)) {
			xmlChar* gidStr = xmlGetProp(child, BAD_CAST("gid"));
			unsigned gid = (unsigned)atoi((const char*)gidStr)-1;
			Tile* t = new Tile;
			t->type = &tilesets[0].defaults[gid]; // XXX can only access first tileset
			t->flags = 0x0;
			t->door = NULL;
			row.push_back(t);
			if (row.size() % dim.x == 0) {
				grid.push_back(row);
				row.clear();
			}
		}
	}

	map.push_back(grid);
	dim.z++;
	return true;
}

bool Area::processObjectGroup(xmlNode* node)
{

/*
 <objectgroup name="Prop0" width="5" height="5">
  <properties>
   <property name="layer" value="0"/>
  </properties>
  <object name="tile2" type="Tile" gid="7" x="64" y="320">
   <properties>
    <property name="onEnter" value="speed(0.5)"/>
    <property name="onLeave" value="undo()"/>
    <property name="door" value="grassfield.area,1,1,0"/>
    <property name="flags" value="npc_nowalk"/>
   </properties>
  </object>
 </objectgroup>
*/

	xmlChar* width = xmlGetProp(node, BAD_CAST("width"));
	xmlChar* height = xmlGetProp(node, BAD_CAST("height"));
	int x = atoi((const char*)width);
	int y = atoi((const char*)height);

	int zpos = -1;

	if (dim.x != x || dim.y != y) {
		// XXX we need to know the Area we're loading...
		Log::err("unknown area", "objectgroup x,y size != map x,y size");
		return false;
	}

	xmlNode* child = node->xmlChildrenNode;
	for (; child != NULL; child = child->next) {
		if (!xmlStrncmp(child->name, BAD_CAST("properties"), 11)) {
			if (!processObjectGroupProperties(child, &zpos))
				return false;
		}
		else if (!xmlStrncmp(child->name, BAD_CAST("object"), 7)) {
			if (zpos == -1 || !processObject(child, zpos))
				return false;
		}
	}

	return true;
}

bool Area::processObjectGroupProperties(xmlNode* node, int* zpos)
{

/*
  <properties>
   <property name="layer" value="0"/>
  </properties>
*/

	xmlNode* child = node->xmlChildrenNode;
	for (; child != NULL; child = child->next) {
		xmlChar* name = xmlGetProp(child, BAD_CAST("name"));
		xmlChar* value = xmlGetProp(child, BAD_CAST("value"));
		if (!xmlStrncmp(name, BAD_CAST("layer"), 6)) {
			int layer = atoi((const char*)value);
			if (0 < layer || layer >= (int)dim.z) {
				// XXX we need to know the Area we're loading...
				Log::err("unknown area",
					"objectgroup must correspond with layer"
				);
				return false;
			}
			*zpos = layer;
		}
	}
	return true;
}

bool Area::processObject(xmlNode* node, int zpos)
{

/*
  <object name="tile2" type="Tile" gid="7" x="64" y="320">
   <properties>
    <property name="onEnter" value="speed(0.5)"/>
    <property name="onLeave" value="undo()"/>
    <property name="door" value="grassfield.area,1,1,0"/>
    <property name="flags" value="npc_nowalk"/>
   </properties>
  </object>
*/

	xmlChar* type = xmlGetProp(node, BAD_CAST("type"));
	if (xmlStrncmp(type, BAD_CAST("Tile"), 5)) {
		Log::err("unknown area", "object type must be Tile");
		return false;
	}

	xmlChar* xStr = xmlGetProp(node, BAD_CAST("x"));
	xmlChar* yStr = xmlGetProp(node, BAD_CAST("y"));
	// XXX we ignore the object gid... is that okay?

	// wouldn't have to access tilesets if we had tiledim ourselves
	long x = atol((const char*)xStr) / tilesets[0].tiledim.x;
	long y = atol((const char*)yStr) / tilesets[0].tiledim.y;
	y = y - 1; // bug in tiled? y is 1 too high

	// We know which Tile is being talked about now... yay
	Tile* t = map[zpos][y][x];

	xmlNode* child = node->xmlChildrenNode; // <properties>
	child = child->xmlChildrenNode; // <property>
	for (; child != NULL; child = child->next) {
		xmlChar* name = xmlGetProp(child, BAD_CAST("name"));
		xmlChar* value = xmlGetProp(child, BAD_CAST("value"));
		if (!xmlStrncmp(name, BAD_CAST("flags"), 6)) {
			t->flags = splitTileFlags((const char*)value);
		}
		else if (!xmlStrncmp(name, BAD_CAST("onEnter"), 8)) {
			// TODO events
		}
		else if (!xmlStrncmp(name, BAD_CAST("onLeave"), 8)) {
			// TODO events
		}
		else if (!xmlStrncmp(name, BAD_CAST("door"), 5)) {
			t->door = parseDoor((const char*)value);
		}
	}
	return true;
}

unsigned Area::splitTileFlags(const std::string strOfFlags)
{
	std::vector<std::string> strs;
	strs = splitStr(strOfFlags, ",");

	unsigned flags = 0x0;
	BOOST_FOREACH(std::string str, strs)
	{
		// TODO: reimplement comparisons as a hash table
		if (str == "nowalk") {
			flags |= nowalk;
		}
	}
	return flags;
}

Area::Door* Area::parseDoor(const std::string dest)
{
	std::vector<std::string> strs;
	strs = splitStr(dest, ",");

	// TODO: verify the validity of the input string... it's coming from
	// user land
	Door* door = new Door;
	door->area = strs[0];
	door->coord.x = atol(strs[1].c_str());
	door->coord.y = atol(strs[2].c_str());
	door->coord.z = atol(strs[3].c_str());
	return door;
}

coord_t Area::getDimensions() const
{
	return dim;
}

Area::Tile* Area::getTile(coord_t c)
{
	return map[c.z][c.y][c.x];
}

