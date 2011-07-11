/******************************
** Tsunagari Tile Engine     **
** area.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <Gosu/Graphics.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/Math.hpp>
#include <Gosu/Timing.hpp>

#include "area.h"
#include "common.h"
#include "entity.h"
#include "log.h"
#include "resourcer.h"
#include "window.h"
#include "world.h"

/* NOTE: TileSet tiles start counting their positions from 0, while layer tiles
         start counting from 1. I can't imagine why the author did this, but we
         have to take it into account. 
*/

Area::Area(Resourcer* rc,
           World* world,
           Player* player,
           const std::string& descriptor)
	: rc(rc), world(world), player(player), descriptor(descriptor),
	  onIntro(false)
{
	dim.x = dim.y = dim.z = 0;
}

Area::~Area()
{
	if (musicInst && musicInst->playing())
		musicInst->stop();
}

bool Area::init()
{
	if (!processDescriptor())
		return false;

	if (introMusic) {
		musicInst.reset(introMusic->play(1, 1, false));
		onIntro = true;
	}
	else if (mainMusic) {
		musicInst.reset(mainMusic->play(1, 1, true));
	}
	return true;
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

	if (attemptingMove)
		player->moveByTile(posMove);
}

void Area::draw()
{
	GameWindow* window = GameWindow::getWindow();
	Gosu::Graphics& graphics = window->graphics();
	const Gosu::Transform trans = translateCoords();
	graphics.pushTransform(trans);

	// Calculate frame to show for each type of tile
	int millis = (int)Gosu::milliseconds();
	BOOST_FOREACH(TileSet& set, tilesets) {
		BOOST_FOREACH(TileType& type, set.tileTypes) {
			if (type.animated) {
				int frame = (millis % type.animLen) /
					type.frameLen;
				type.frameShowing = frame;
				type.graphic = type.graphics[frame].get();
			}
		}
	}

	for (unsigned layer = 0; layer != map.size(); layer++)
	{
		grid_t grid = map[layer];
		for (unsigned y = 0; y != grid.size(); y++)
		{
			row_t row = grid[y];
			for (unsigned x = 0; x != row.size(); x++)
			{
				const Tile& tile = row[x];
				const TileType* type = tile.type;
				const Gosu::Image* img = type->graphic;
				img->draw(x*img->width(), y*img->height(), 0);
			}
		}
	}
	player->draw();

	graphics.popTransform();
}

bool Area::needsRedraw() const
{
	if (player->needsRedraw())
		return true;

	// Do any types of tiles need to update their animations?
	int millis = (int)Gosu::milliseconds();
	BOOST_FOREACH(const TileSet& set, tilesets) {
		BOOST_FOREACH(const TileType& type, set.tileTypes) {
			if (type.animated) {
				int frame = (millis % type.animLen) /
					type.frameLen;
				if (frame != type.frameShowing)
					return true;
			}
		}
	}

	return false;
}

void Area::update()
{
	if (onIntro && !musicInst->playing()) {
		onIntro = false;
		musicInst.reset(mainMusic->play(1, 1, true));
	}
	player->update();
}

static double center(double w, double g, double p)
{
	return w>g ? (w-g)/2.0 : Gosu::boundBy(w/2.0-p, w-g, 0.0);
}

Gosu::Transform Area::translateCoords()
{
	GameWindow* window = GameWindow::getWindow();
	Gosu::Graphics* graphics = &window->graphics();

	double tileWidth = (double)tilesets[0].tileDim.x;
	double tileHeight = (double)tilesets[0].tileDim.y;
	double windowWidth = (double)graphics->width() / tileWidth;
	double windowHeight = (double)graphics->height() / tileHeight;
	double gridWidth = (double)dim.x;
	double gridHeight = (double)dim.y;
	double playerX = (double)player->getCoordsByPixel().x /
			tileWidth + 0.5;
	double playerY = (double)player->getCoordsByPixel().y /
			tileHeight + 0.5;

	coord_t c;
	c.x = (long)(center(windowWidth, gridWidth, playerX) * tileWidth);
	c.y = (long)(center(windowHeight, gridHeight, playerY) * tileHeight);

	Gosu::Transform trans = Gosu::translate((double)c.x, (double)c.y);
	return trans;
}

bool Area::processDescriptor()
{
	XMLDocRef doc = rc->getXMLDoc(descriptor, "dtd/area.dtd");
	if (!doc)
		return false;

	// Iterate and process children of <map>
	xmlNode* root = xmlDocGetRootElement(doc.get()); // <map> element

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
			if (!processTileSet(child))
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
  <property name="intro_music" value="intro.music"/>
  <property name="main_music" value="wind.music"/>
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
		else if (!xmlStrncmp(name, BAD_CAST("intro_music"), 12))
			introMusic = rc->getSample((const char*)value);
		else if (!xmlStrncmp(name, BAD_CAST("main_music"), 11))
			mainMusic = rc->getSample((const char*)value);
		else if (!xmlStrncmp(name, BAD_CAST("onLoad"), 7))
			onLoadEvents = (const char*)value;
		else if (!xmlStrncmp(name, BAD_CAST("scripts"), 8))
			scripts = (const char*)value; // TODO split(), load
	}
	return true;
}

bool Area::processTileSet(xmlNode* node)
{

/*	
 <tileset firstgid="1" name="tiles.sheet" tilewidth="64" tileheight="64">
  <image source="tiles.sheet" width="256" height="256"/>
  <tile id="14">
   ...
  </tile>
 </tileset>
*/
	TileSet ts;
	xmlChar* width = xmlGetProp(node, BAD_CAST("tilewidth"));
	xmlChar* height = xmlGetProp(node, BAD_CAST("tileheight"));
	long x = ts.tileDim.x = atol((const char*)width);
	long y = ts.tileDim.y = atol((const char*)height);
	
	xmlNode* child = node->xmlChildrenNode;
	for (; child != NULL; child = child->next) {
		if (!xmlStrncmp(child->name, BAD_CAST("tile"), 5)) {
			xmlChar* idstr = xmlGetProp(child, BAD_CAST("id"));
			unsigned id = (unsigned)atoi((const char*)idstr);

			// Undeclared TileTypes have default properties.
			while (ts.tileTypes.size() != id) {
				TileType tt = defaultTileType(ts);
				ts.tileTypes.push_back(tt);
			}

			// Handle explicit TileType
			if (!processTileType(child, ts))
				return false;
		}
		else if (!xmlStrncmp(child->name, BAD_CAST("image"), 6)) {
			const char* source = (const char*)xmlGetProp(child,
				BAD_CAST("source"));
			rc->getTiledImage(ts.tiles, source,
				(unsigned)x, (unsigned)y, true);
		}
	}

	while (ts.tiles.size()) {
		TileType tt = defaultTileType(ts);
		ts.tileTypes.push_back(tt);
	}

	tilesets.push_back(ts);
	return true;
}

Area::TileType Area::defaultTileType(TileSet& set)
{
	TileType type;
	type.animated = false;
	type.frameLen = 1000;
	type.flags = 0x0;
	type.graphics.push_back(set.tiles.front());
	type.graphic = type.graphics[0].get();
	set.tiles.pop_front();
	return type;
}

bool Area::processTileType(xmlNode* node, TileSet& ts)
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
	TileType tt = defaultTileType(ts);

	xmlChar* idstr = xmlGetProp(node, BAD_CAST("id"));
	unsigned id = (unsigned)atoi((const char*)idstr); // atoi
	long expectedId = ts.tileTypes.size();
	if (id != expectedId) {
		Log::err(descriptor, std::string("expected TileType id ") +
		         itostr(expectedId) + ", but got " +
			 itostr(id));
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
			int size = atoi((const char*)value); // atoi

			// Add size-1 more frames to our animation.
			// We already have one from defaultTileType.
			for (int i = 1; i < size; i++) {
				if (ts.tiles.empty()) {
					Log::err(descriptor, "ran out of tiles"
						"/frames for animated tile");
					return false;
				}
				tt.graphics.push_back(ts.tiles.front());
				ts.tiles.pop_front();
			}
			tt.animLen = tt.frameLen * (int)tt.graphics.size();
		}
		else if (!xmlStrncmp(name, BAD_CAST("speed"), 6)) {
			tt.frameLen = (int)(1000.0/atof((const char*)value));
			tt.animLen = tt.frameLen * (int)tt.graphics.size();
		}
	}

	ts.tileTypes.push_back(tt);
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
		Log::err(descriptor, "layer x,y size != map x,y size");
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
				Log::err(descriptor, "invalid layer depth");
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

	row.reserve(dim.x);
	grid.reserve(dim.y);

	xmlNode* child = node->xmlChildrenNode;
	for (int i = 1; child != NULL; i++, child = child->next) {
		if (!xmlStrncmp(child->name, BAD_CAST("tile"), 5)) {
			xmlChar* gidStr = xmlGetProp(child, BAD_CAST("gid"));
			unsigned gid = (unsigned)atoi((const char*)gidStr)-1;

			// XXX can only access first tileset
			TileType* type = &tilesets[0].tileTypes[gid];

			Tile t;
			t.type = type;
			t.flags = 0x0;
			type->allOfType.push_back(&t);
			row.push_back(t);
			if (row.size() % dim.x == 0) {
				grid.push_back(row);
				row.clear();
				row.reserve(dim.x);
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
		Log::err(descriptor,
				"objectgroup x,y size != map x,y size");
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
				Log::err(descriptor,
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
		Log::err(descriptor, "object type must be Tile");
		return false;
	}

	xmlChar* xStr = xmlGetProp(node, BAD_CAST("x"));
	xmlChar* yStr = xmlGetProp(node, BAD_CAST("y"));
	// XXX we ignore the object gid... is that okay?

	// wouldn't have to access tilesets if we had tileDim ourselves
	long x = atol((const char*)xStr) / tilesets[0].tileDim.x;
	long y = atol((const char*)yStr) / tilesets[0].tileDim.y;
	y = y - 1; // bug in tiled? y is 1 too high

	// We know which Tile is being talked about now... yay
	Tile& t = map[zpos][y][x];

	xmlNode* child = node->xmlChildrenNode; // <properties>
	child = child->xmlChildrenNode; // <property>
	for (; child != NULL; child = child->next) {
		xmlChar* name = xmlGetProp(child, BAD_CAST("name"));
		xmlChar* value = xmlGetProp(child, BAD_CAST("value"));
		if (!xmlStrncmp(name, BAD_CAST("flags"), 6)) {
			t.flags = splitTileFlags((const char*)value);
		}
		else if (!xmlStrncmp(name, BAD_CAST("onEnter"), 8)) {
			// TODO events
		}
		else if (!xmlStrncmp(name, BAD_CAST("onLeave"), 8)) {
			// TODO events
		}
		else if (!xmlStrncmp(name, BAD_CAST("door"), 5)) {
			t.door.reset(parseDoor((const char*)value));
			t.flags |= npc_nowalk;
		}
	}
	return true;
}

unsigned Area::splitTileFlags(const std::string strOfFlags)
{
	std::vector<std::string> strs;
	strs = splitStr(strOfFlags, ",");

	unsigned flags = 0x0;
	BOOST_FOREACH(const std::string& str, strs)
	{
		// TODO: reimplement comparisons as a hash table
		if (str == "nowalk") {
			flags |= nowalk;
		}
	}
	return flags;
}

Area::Door Area::parseDoor(const std::string dest)
{

/*
  Format: destination Area, x, y, z
  E.g.:   "babysfirst.area,1,3,0"
*/

	std::vector<std::string> strs;
	strs = splitStr(dest, ",");

	// TODO: verify the validity of the input string... it's coming from
	// user land
	Door door;
	door.area = strs[0];
	door.coord.x = atol(strs[1].c_str());
	door.coord.y = atol(strs[2].c_str());
	door.coord.z = atol(strs[3].c_str());
	return door;
}

coord_t Area::getDimensions() const
{
	return dim;
}

coord_t Area::getTileDimensions() const
{
	return tilesets[0].tileDim; // XXX only considers first tileset
}

Area::Tile& Area::getTile(coord_t c)
{
	return map[c.z][c.y][c.x];
}

