/******************************
** Tsunagari Tile Engine     **
** area.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <math.h>
#include <vector>

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
#include "tile.h"
#include "window.h"
#include "world.h"

#define ASSERT(x)  if (!(x)) return false

// Rename Tile => Square
// Introduce new Tile

/* NOTE: In the TMX map format used by Tiled, tileset tiles start counting
         their Y-positions from 0, while layer tiles start counting from 1. I
         can't imagine why the author did this, but we have to take it into
         account.
*/

Area::Area(Resourcer* rc,
           World* world,
           Viewport* view,
           Player* player,
           Music* music,
           const std::string& descriptor)
	: rc(rc), world(world), view(view), player(player), music(music),
	  descriptor(descriptor), dim(0, 0, 0), loopX(false), loopY(false)
{
}

Area::~Area()
{
}

bool Area::init()
{
	return processDescriptor();
}

void Area::buttonDown(const Gosu::Button btn)
{
	if (btn == Gosu::kbRight)
		player->startMovement(icoord(1, 0, 0));
	else if (btn == Gosu::kbLeft)
		player->startMovement(icoord(-1, 0, 0));
	else if (btn == Gosu::kbUp)
		player->startMovement(icoord(0, -1, 0));
	else if (btn == Gosu::kbDown)
		player->startMovement(icoord(0, 1, 0));
}

void Area::buttonUp(const Gosu::Button btn)
{
	if (btn == Gosu::kbRight)
		player->stopMovement(icoord(1, 0, 0));
	else if (btn == Gosu::kbLeft)
		player->stopMovement(icoord(-1, 0, 0));
	else if (btn == Gosu::kbUp)
		player->stopMovement(icoord(0, -1, 0));
	else if (btn == Gosu::kbDown)
		player->stopMovement(icoord(0, 1, 0));
}

void Area::draw()
{
	Gosu::Graphics& graphics = GameWindow::getWindow().graphics();
	rvec2 off = view->getOffset();
	graphics.pushTransform(Gosu::translate(-off.x, -off.y));

	updateTileAnimations();
	drawTiles();
	drawEntities();

	graphics.popTransform();
}

void Area::updateTileAnimations()
{
	const int millis = GameWindow::getWindow().time();
	BOOST_FOREACH(TileType& type, tileTypes)
		type.anim.updateFrame(millis);
}

bool Area::inBounds(int x, int y, int z) const
{
	return ((loopX || (0 <= x && x < dim.x)) &&
		(loopY || (0 <= y && y < dim.y)) &&
		          0 <= z && z < dim.z);
}

void Area::drawTiles() const
{
	const icube_t tiles = visibleTiles();
	for (int z = tiles.z1; z < tiles.z2; z++) {
		for (int y = tiles.y1; y < tiles.y2; y++) {
			for (int x = tiles.x1; x < tiles.x2; x++) {
				int tx = x, ty = y, tz = z;
				if (loopX)
					tx = wrap(0, tx, dim.x);
				if (loopY)
					ty = wrap(0, ty, dim.y);
				if (inBounds(tx, ty, tz))
					drawTile(map[tz][ty][tx], x, y, z);
			}
		}
	}
}

void Area::drawTile(const Tile& tile, int x, int y, int) const
{
	BOOST_FOREACH(const Block& block, tile.blocks) {
		const Gosu::Image* img = block.type->anim.frame();
		if (img)
			img->draw((double)x*img->width(),
			          (double)y*img->height(), block.depth);
	}
}

void Area::drawEntities()
{
	player->draw();
}

bool Area::needsRedraw() const
{
	if (player->needsRedraw())
		return true;

	// Do any onscreen tile types need to update their animations?
	BOOST_FOREACH(const TileType& type, tileTypes)
		if (type.needsRedraw(*this))
			return true;
	return false;
}

void Area::update(unsigned long dt)
{
	music->update();
	player->update(dt);
	view->update(dt);
}

icoord Area::getDimensions() const
{
	return dim;
}

icoord Area::getTileDimensions() const
{
	return tileDim;
}

bool Area::tileExists(icoord c) const
{
	return inBounds(c.x, c.y, c.z);
}

const Tile& Area::getTile(icoord c) const
{
	if (loopX)
		c.x = wrap(0, c.x, dim.x);
	if (loopY)
		c.y = wrap(0, c.y, dim.y);
	return map[c.z][c.y][c.x];
}

Tile& Area::getTile(icoord c)
{
	if (loopX)
		c.x = wrap(0, c.x, dim.x);
	if (loopY)
		c.y = wrap(0, c.y, dim.y);
	return map[c.z][c.y][c.x];
}

icube_t Area::visibleTiles() const
{
	const Gosu::Graphics& graphics = GameWindow::getWindow().graphics();
	const int tileWidth = tileDim.x;
	const int tileHeight = tileDim.y;
	const int windowWidth = graphics.width();
	const int windowHeight = graphics.height();
	const rvec2 off = view->getOffset();

	const int x1 = (int)floor(off.x / tileWidth);
	const int y1 = (int)floor(off.y / tileHeight);
	const int x2 = (int)ceil((double)(windowWidth + off.x) /
		(double)tileWidth);
	const int y2 = (int)ceil((double)(windowHeight + off.y) /
		(double)tileHeight);

	return icube(x1, y1, 0, x2, y2, dim.z);
}

bool Area::loopsInX() const
{
	return loopX;
}

bool Area::loopsInY() const
{
	return loopY;
}

bool Area::processDescriptor()
{
	XMLRef doc;
	XMLNode root;

	ASSERT(doc = rc->getXMLDoc(descriptor, "area.dtd"));
	ASSERT(root = doc->root()); // <map>

	ASSERT(root.intAttr("width", &dim.x));
	ASSERT(root.intAttr("height", &dim.y));
	dim.z = 1;

	allocateMap();

	for (XMLNode child = root.childrenNode(); child; child = child.next()) {
		if (child.is("properties")) {
			ASSERT(processMapProperties(child));
		}
		else if (child.is("tileset")) {
			ASSERT(processTileSet(child));
		}
		else if (child.is("layer")) {
			ASSERT(processLayer(child));
		}
		else if (child.is("objectgroup")) {
			ASSERT(processObjectGroup(child));
		}
	}

	return true;
}

void Area::allocateMap()
{
	map.push_back(grid_t(dim.y, row_t(dim.x)));
}

bool Area::processMapProperties(XMLNode node)
{

/*
 <properties>
  <property name="author" value="Michael D. Reiley"/>
  <property name="name" value="Baby's First Area"/>
  <property name="intro_music" value="intro.music"/>
  <property name="main_music" value="wind.music"/>
  <property name="onLoad" value="babysfirst_init()"/>
  <property name="scripts" value="areainits.event,test.event"/>
  <property name="loop" value="xy"/>
 </properties>
*/
	bool introSet = false;
	bool mainSet = false;

	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		std::string name = child.attr("name");
		std::string value = child.attr("value");
		if (name == "author")
			author = value;
		else if (name == "name")
			this->name = value;
		else if (name == "intro_music") {
			music->setIntro(value);
			introSet = true;
		}
		else if (name == "main_music") {
			music->setMain(value);
			mainSet = true;
		}
		else if (name == "onLoad")
			onLoadEvents = value;
		else if (name == "scripts")
			scripts = value; // TODO split(), load
		else if (name == "loop") {
			loopX = value.find('x') != std::string::npos;
			loopY = value.find('y') != std::string::npos;
		}
	}

	if (!introSet)
		music->setIntro("");
	if (!mainSet)
		music->setMain("");
	return true;
}

bool Area::processTileSet(XMLNode node)
{

/*
 <tileset firstgid="1" name="tiles.sheet" tilewidth="64" tileheight="64">
  <image source="tiles.sheet" width="256" height="256"/>
  <tile id="14">
   ...
  </tile>
 </tileset>
*/

	int x, y, z;

	ASSERT(node.intAttr("tilewidth", &x));
	ASSERT(node.intAttr("tileheight", &y));
	z = 1;

	TiledImage img;
	tileDim = icoord(x, y, z);
	// FIXME: compare with existing tileDim

	if (tileTypes.empty()) {
		// Add TileType #0, a transparent tile type that fills map
		// squares for sections of the map that don't exist.
		TileType zero;
		zero.flags = nowalk;
		tileTypes.push_back(zero);
		// XXX: Paul 2011-11-13: This tiletype isn't directly used
		//      anymore. Should we remove it?
	}

	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		if (child.is("image")) {
			std::string source = child.attr("source");
			rc->getTiledImage(img, source,
				(unsigned)x, (unsigned)y, true);
		}
		else if (child.is("tile")) {
			// FIXME: Ensure img
			if (img.empty()) {
				Log::err(descriptor,
				  "Tile processed before tileset image loaded");
				return false;
			}

			int id;
			ASSERT(child.intAttr("id", &id));

			if (id < 0 || (int)tileTypes.size() +
			              (int)img.size() <= id) {
				Log::err(descriptor, "tile type id is invalid");
				return false;
			}

			// Type ids are given with offset given in terms of
			// tile from the image rather than in terms of the gid
			// number, which has an extra type, #0, pushing back
			// all other types by one.
			int gid = id + 1;

			if ((int)tileTypes.size() > gid) {
				Log::err(descriptor,
					"tile types must be sorted by id");
				return false;
			}

			// Undeclared types have default properties.
			while ((int)tileTypes.size() < gid)
				tileTypes.push_back(TileType(img));

			// Handle this (explicitly declared) type.
			ASSERT(processTileType(child, img, id));
		}
	}

	// Handle remaining anonymous items.
	while (img.size())
		tileTypes.push_back(TileType(img));
	return true;
}

bool Area::processTileType(XMLNode node, TiledImage& img, int id)
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
    <property name="members" value="1,2,3,4"/>
    <property name="speed" value="2"/>
   </properties>
  </tile>
*/

	// The id has already been handled by processTileSet, so we don't have
	// to worry about it.

	// Initialize a default TileType, we'll build on that.
	TileType type(img);

	XMLNode child = node.childrenNode(); // <properties>
	for (child = child.childrenNode(); child; child = child.next()) {
		// Each <property>...
		std::string name = child.attr("name");
		std::string value = child.attr("value");
		if (name == "flags") {
			type.flags = splitTileFlags(value);
		}
		else if (name == "onEnter") {
			if (!rc->resourceExists(value)) {
				Log::err("Resourcer", "script " + value +
						" referenced but not found");
				continue;
			}
			TileEvent e;
			e.trigger = onEnter;
			e.script = value;
			type.events.push_back(e);
			type.flags |= hasOnEnter;
		}
		else if (name == "onLeave") {
			if (!rc->resourceExists(value)) {
				Log::err("Resourcer", "script " + value +
						" referenced but not found");
				continue;
			}
			TileEvent e;
			e.trigger = onLeave;
			e.script = value;
			type.events.push_back(e);
			type.flags |= hasOnLeave;
		}
		else if (name == "members") {
			std::string memtemp;
			std::vector<std::string> members;
			std::vector<std::string>::iterator it;
			memtemp = value;
			members = splitStr(memtemp, ",");
			
			// Make sure the first member is this tile.
			if (atoi(members[0].c_str()) != id) {
				Log::err(descriptor, "first member of tile"
					" id " + itostr(id) + " animation must be itself.");
				return false;
			}

			// Add frames to our animation.
			// We already have one from TileType's constructor.
			for (it = members.begin()+1; it < members.end(); it++) {
				if (img.empty()) {
					Log::err(descriptor, "ran out of tiles"
						"/frames for animated tile");
					return false;
				}
				type.anim.addFrame(img[id-atoi(it->c_str())+1]);
			}
		}
		else if (name == "speed") {
			double hertz;
			ASSERT(child.doubleAttr("value", &hertz));
			int len = (int)(1000.0/hertz);
			type.anim.setFrameLen(len);
		}
	}

	tileTypes.push_back(type);
	return true;
}

bool Area::processLayer(XMLNode node)
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

	int x, y;
	double depth;
	ASSERT(node.intAttr("width", &x));
	ASSERT(node.intAttr("height", &y));

	if (dim.x != x || dim.y != y) {
		Log::err(descriptor, "layer x,y size != map x,y size");
		return false;
	}

	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		if (child.is("properties")) {
			ASSERT(processLayerProperties(child, &depth));
		}
		else if (child.is("data")) {
			ASSERT(processLayerData(child, depth));
		}
	}
	return true;
}

bool Area::processLayerProperties(XMLNode node, double* depth)
{

/*
  <properties>
   <property name="layer" value="0"/>
  </properties>
*/

	// FIXME: REQUIRE layer key.
	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		std::string name  = child.attr("name");
		std::string value = child.attr("value");
		if (name == "layer") {
			ASSERT(child.doubleAttr("value", depth));
			// XXX: assert depth hasn't already been used
		}
	}

	return true;
}

bool Area::processLayerData(XMLNode node, double depth)
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

	int x = 0, y = 0;

	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		if (child.is("tile")) {
			int gid;
			ASSERT(child.intAttr("gid", &gid));

			if (gid < 0 || (int)tileTypes.size() <= gid) {
				Log::err(descriptor, "invalid tile gid");
				return false;
			}

			// A gid of zero means there is no tile at this
			// position on this layer.
			if (gid > 0) {
				TileType& type = tileTypes[gid];
				Tile& tile = map[0][y][x];

				Block block(depth, &type);
				tile.blocks.push_back(block);
				type.allOfType.push_back(&tile);
			}

			if (++x == dim.x) {
				x = 0;
				y++;
			}
		}
	}

	return true;
}

bool Area::processObjectGroup(XMLNode node)
{

/*
 <objectgroup name="Prop0" width="5" height="5">
  <properties>
   <property name="layer" value="0"/>
  </properties>
  <object name="tile2" gid="7" x="64" y="320">
   <properties>
    <property name="onEnter" value="speed(0.5)"/>
    <property name="onLeave" value="undo()"/>
    <property name="door" value="grassfield.area,1,1,0"/>
    <property name="flags" value="npc_nowalk"/>
   </properties>
  </object>
 </objectgroup>
*/

	int x, y;
	ASSERT(node.intAttr("width", &x));
	ASSERT(node.intAttr("height", &y));

	int zpos = -1;

	if (dim.x != x || dim.y != y) {
		Log::err(descriptor, "objectgroup x,y size != map x,y size");
		return false;
	}

	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		if (child.is("properties")) {
			ASSERT(processObjectGroupProperties(child, &zpos));
		}
		else if (child.is("object")) {
			ASSERT(zpos != -1 && processObject(child, zpos));
		}
	}

	return true;
}

bool Area::processObjectGroupProperties(XMLNode node, int* zpos)
{

/*
  <properties>
   <property name="layer" value="0"/>
  </properties>
*/

	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		std::string name = child.attr("name");
		std::string value = child.attr("value");
		if (name == "layer") {
			int layer;
			ASSERT(child.intAttr("value", &layer));
//			if (layer < 0 || (int)dim.z <= layer) {
//				Log::err(descriptor,
//					"objectgroup must correspond with layer"
//				);
//				return false;
//			}
			*zpos = layer;
		}
	}
	return true;
}

bool Area::processObject(XMLNode node, int zpos)
{

/*
  <object name="tile2" gid="7" x="64" y="320">
   <properties>
    <property name="onEnter" value="speed(0.5)"/>
    <property name="onLeave" value="undo()"/>
    <property name="door" value="grassfield.area,1,1,0"/>
    <property name="flags" value="npc_nowalk"/>
   </properties>
  </object>
  <object name="foo" x="0" y="0" width="64" height="64">
   ...
  </object>
*/

	// Gather object properties now. Assign them to tiles later.
	std::vector<TileEvent> events;
	boost::optional<Door> door;
	unsigned flags = 0x0;

	XMLNode child = node.childrenNode(); // <properties>
	for (child = child.childrenNode(); child; child = child.next()) {
		// Each <property>...
		std::string name = child.attr("name");
		std::string value = child.attr("value");
		if (name == "flags") {
			flags = splitTileFlags(value);
		}
		else if (name == "onEnter") {
			if (!rc->resourceExists(value)) {
				Log::err("Resourcer", "script " + value +
						" referenced but not found");
				continue;
			}
			TileEvent e;
			e.trigger = onEnter;
			e.script = value;
			events.push_back(e);
			flags |= hasOnEnter;
		}
		else if (name == "onLeave") {
			if (!rc->resourceExists(value)) {
				Log::err("Resourcer", "script " + value +
						" referenced but not found");
				continue;
			}
			TileEvent e;
			e.trigger = onLeave;
			e.script = value;
			events.push_back(e);
			flags |= hasOnLeave;
		}
		else if (name == "door") {
			door.reset(parseDoor(value));
			flags |= npc_nowalk;
		}
	}

	// Apply these properties directly to one or more tiles in a rectangle
	// of the map. We don't keep an intermediary "object" object lying
	// around.
	int x, y, w, h;
	ASSERT(node.intAttr("x", &x));
	ASSERT(node.intAttr("y", &y));
	x /= tileDim.x;
	y /= tileDim.y;

	if (node.hasAttr("gid")) {
		// This is one of Tiled's "Tile Objects". It is one tile wide
		// and high.
		y = y - 1; // Bug in tiled. The y is off by one.
		w = 1;
		h = 1;

		// We don't actually use the object gid. It is supposed to indicate
		// which tile our object is rendered as, but, for Tsunagari, tile
		// objects are always transparent and reveal the tile below.
	}
	else {
		// This is one of Tiled's "Objects". It has a width and height.
		ASSERT(node.intAttr("width", &w));
		ASSERT(node.intAttr("height", &h));
		w /= tileDim.x;
		h /= tileDim.y;
	}

	// We know which Tiles are being talked about now... yay
	for (int Y = y; Y < y + h; Y++) {
		for (int X = x; X < x + w; X++) {
			Tile& tile = map[zpos][Y][X];

			tile.flags |= flags;
			if (door)
				tile.door = door;
			BOOST_FOREACH(TileEvent& e, events)
				tile.events.push_back(e);
		}
	}

	return true;
}

// FIXME: It can fail, should return bool.
unsigned Area::splitTileFlags(const std::string strOfFlags)
{
	std::vector<std::string> strs;
	strs = splitStr(strOfFlags, ",");

	unsigned flags = 0x0;
	BOOST_FOREACH(const std::string& str, strs) {
		if (str == "nowalk")
			flags |= nowalk;
	}
	return flags;
}

// FIXME: It can fail, should return bool.
Door Area::parseDoor(const std::string dest)
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
	door.tile.x = atoi(strs[1].c_str());
	door.tile.y = atoi(strs[2].c_str());
	door.tile.z = atoi(strs[3].c_str());
	return door;
}

