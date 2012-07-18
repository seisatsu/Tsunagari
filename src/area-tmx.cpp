/*********************************
** Tsunagari Tile Engine        **
** area-tmx.cpp                 **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <math.h>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <Gosu/Graphics.hpp>
#include <Gosu/Image.hpp>
#include <Gosu/Math.hpp>
#include <Gosu/Timing.hpp>

#include "area-tmx.h"
#include "entity.h"
#include "log.h"
#include "python.h"
#include "resourcer.h"
#include "string.h"
#include "tile.h"
#include "window.h"
#include "world.h"

#ifdef _WIN32
	#include "os-windows.h"
#endif

#define ASSERT(x)  if (!(x)) { return false; }

/* NOTE: In the TMX map format used by Tiled, tileset tiles start counting
         their Y-positions from 0, while layer tiles start counting from 1. I
         can't imagine why the author did this, but we have to take it into
         account.
*/

AreaTMX::AreaTMX(Viewport* view,
           Player* player,
           Music* music,
           const std::string& descriptor)
	: Area(view, player, music, descriptor)
{
	// Add TileType #0. Not used, but Tiled's gids start from 1.
	gids.push_back(NULL);
}

AreaTMX::~AreaTMX()
{
}

bool AreaTMX::init()
{
	return processDescriptor();
}


void AreaTMX::allocateMapLayer()
{
	map.push_back(grid_t(dim.y, row_t(dim.x)));
	grid_t& grid = map[dim.z];
	for (int y = 0; y < dim.y; y++) {
		row_t& row = grid[y];
		for (int x = 0; x < dim.x; x++) {
			Tile& tile = row[x];
			new (&tile) Tile(this, x, y, dim.z);
		}
	}
	dim.z++;
}

bool AreaTMX::processDescriptor()
{
	XMLRef doc;
	XMLNode root;

	Resourcer* rc = Resourcer::instance();
	ASSERT(doc = rc->getXMLDoc(descriptor, "dtd/area.dtd"));
	ASSERT(root = doc->root()); // <map>

	ASSERT(root.intAttr("width", &dim.x));
	ASSERT(root.intAttr("height", &dim.y));
	dim.z = 0;

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

bool AreaTMX::processMapProperties(XMLNode node)
{

/*
 <properties>
  <property name="author" value="Random J. Hacker"/>
  <property name="name" value="Wooded AreaTMX"/>
  <property name="intro_music" value="arrive.ogg"/>
  <property name="main_music" value="wind.ogg"/>
  <property name="on_load" value="wood_setup.py"/>
  <property name="on_focus" value="wood_focus.py"/>
  <property name="on_tick" value="wood_tick.py"/>
  <property name="on_turn" value="wood_turn.py"/>
  <property name="loop" value="xy"/>
  <property name="color_overlay" value="255,255,255,127"/>
 </properties>
*/
	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		std::string name = child.attr("name");
		std::string value = child.attr("value");
		if (name == "author")
			author = value;
		else if (name == "name")
			this->name = value;
		else if (name == "intro_music") {
			musicIntro.reset(value);
		}
		else if (name == "main_music") {
			musicLoop.reset(value);
		}
		else if (name == "on_load") {
			std::string filename = value;
			ScriptInst script(filename);
			if (!script.validate(descriptor))
				return false;
			loadScript = filename;
		}
		else if (name == "on_focus") {
			std::string filename = value;
			ScriptInst script(filename);
			if (!script.validate(descriptor))
				return false;
			focusScript = filename;
		}
		else if (name == "on_tick") {
			std::string filename = value;
			ScriptInst script(filename);
			if (!script.validate(descriptor))
				return false;
			tickScript = filename;
		}
		else if (name == "on_turn") {
			std::string filename = value;
			ScriptInst script(filename);
			if (!script.validate(descriptor))
				return false;
			turnScript = filename;
		}
		else if (name == "loop") {
			loopX = value.find('x') != std::string::npos;
			loopY = value.find('y') != std::string::npos;
		}
		else if (name == "color_overlay") {
			Gosu::Color::Channel r, g, b, a;
			ASSERT(parseRGBA(value, &r, &g, &b, &a));
			colorOverlay = Gosu::Color(a, r, g, b);
		}
	}

	return true;
}

bool AreaTMX::processTileSet(XMLNode node)
{

/*
 <tileset firstgid="1" name="tiles.sheet" tilewidth="64" tileheight="64">
  <image source="tiles.sheet" width="256" height="256"/>
  <tile id="14">
   ...
  </tile>
 </tileset>
*/

	TileSet* set;
	TiledImage img;
	int tilex, tiley;
	int firstGid;

	ASSERT(node.intAttr("tilewidth", &tilex));
	ASSERT(node.intAttr("tileheight", &tiley));
	ASSERT(node.intAttr("firstgid", &firstGid));

	if (tileDim && tileDim != ivec2(tilex, tiley)) {
		Log::err(descriptor,
			"<tileset>'s width/height contradict earlier <layer>");
		return false;
	}
	tileDim = ivec2(tilex, tiley);

	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		if (child.is("image")) {
			int pixelw, pixelh;
			ASSERT(child.intAttr("width", &pixelw) &&
			       child.intAttr("height", &pixelh));
			int width = pixelw / tileDim.x;
			int height = pixelh / tileDim.y;

			std::string source = child.attr("source");
			tileSets[source] = TileSet(width, height);
			set = &tileSets[source];

			// Load tileset image.
			Resourcer* rc = Resourcer::instance();
			bool found = rc->getTiledImage(img, source,
				(unsigned)tilex, (unsigned)tiley, true);
			if (!found) {
				Log::err(descriptor, "tileset image not found");
				return false;
			}

			// Initialize "vanilla" tile type array.
			BOOST_FOREACH(ImageRef& tileImg, img) {
				TileType* type = new TileType(tileImg);
				set->add(type);
				gids.push_back(type);
			}
		}
		else if (child.is("tile")) {
			// Handle an explicitly declared "non-vanilla" type.

			if (img.empty()) {
				Log::err(descriptor,
				  "Tile type processed before tileset image loaded");
				return false;
			}

			// "id" is 0-based index of a tile in the current
			// tileset, if the tileset were a flat array.
			int id;
			ASSERT(child.intAttr("id", &id));

			if (id < 0 || (int)img.size() <= id) {
				Log::err(descriptor, "tile type id is invalid");
				return false;
			}

			// Initialize a default TileType, we'll build on that.
			TileType* type = new TileType(img[id]);
			ASSERT(processTileType(child, *type, img, id));

			// "gid" is the global area-wide id of the tile.
			size_t gid = id + firstGid;
			delete gids[gid]; // "vanilla" type
			gids[gid] = type;
			set->set(id, type);
		}
	}

	return true;
}

bool AreaTMX::processTileType(XMLNode node, TileType& type, TiledImage& img, int id)
{

/*
  <tile id="8">
   <properties>
    <property name="flags" value="nowalk"/>
    <property name="onEnter" value="skid();speed(2)"/>
    <property name="onLeave" value="undo()"/>
    <property name="onUse" value="undo()"/>
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

	// If a Tile is animated, it needs both member frames and a speed.
	std::vector<ImageRef> frames;
	int frameLen = -1;

	XMLNode child = node.childrenNode(); // <properties>
	for (child = child.childrenNode(); child; child = child.next()) {
		// Each <property>...
		std::string name = child.attr("name");
		std::string value = child.attr("value");
		if (name == "flags") {
			ASSERT(splitTileFlags(value, &type.flags));
		}
		else if (name == "on_enter") {
			std::string filename = value;
			ScriptInst script(filename);
			if (!script.validate(descriptor))
				return false;
			type.enterScript = filename;
		}
		else if (name == "on_leave") {
			std::string filename = value;
			ScriptInst script(filename);
			if (!script.validate(descriptor))
				return false;
			type.leaveScript = filename;
		}
		else if (name == "on_use") {
			std::string filename = value;
			ScriptInst script(filename);
			if (!script.validate(descriptor))
				return false;
			type.useScript = filename;
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
					" id " + itostr(id) +
					" animation must be itself.");
				return false;
			}

			// Add frames to our animation.
			// We already have one from TileType's constructor.
			for (it = members.begin(); it < members.end(); it++) {
				int idx = atoi(it->c_str());
				if (idx < 0 || (int)img.size() <= idx) {
					Log::err(descriptor, "frame index out "
						"of range for animated tile");
					return false;
				}
				frames.push_back(img[idx]);
			}
		}
		else if (name == "speed") {
			double hertz;
			ASSERT(child.doubleAttr("value", &hertz));
			frameLen = (int)(1000.0/hertz);
		}
	}

	if (frames.size() || frameLen != -1) {
		if (frames.empty() || frameLen == -1) {
			Log::err(descriptor, "tile type must either have both "
				"members and speed or none");
			return false;
		}
		type.anim = Animation(frames, frameLen);
	}

	return true;
}

bool AreaTMX::processLayer(XMLNode node)
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

	allocateMapLayer();

	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		if (child.is("properties")) {
			ASSERT(processLayerProperties(child, &depth));
		}
		else if (child.is("data")) {
			ASSERT(processLayerData(child, dim.z - 1));
		}
	}

	return true;
}

bool AreaTMX::processLayerProperties(XMLNode node, double* depth)
{

/*
  <properties>
   <property name="layer" value="0"/>
  </properties>
*/

	bool layerFound = false;

	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		std::string name  = child.attr("name");
		std::string value = child.attr("value");
		if (name == "layer") {
			layerFound = true;
			ASSERT(child.doubleAttr("value", depth));
			if (depth2idx.find(*depth) != depth2idx.end()) {
				Log::err(descriptor,
				         "depth used multiple times");
				return false;
			}

			depth2idx[*depth] = dim.z - 1;
			idx2depth.push_back(*depth);
			// Effectively idx2depth[dim.z - 1] = depth;
		}
	}

	if (!layerFound)
		Log::err(descriptor, "<layer> must have layer property");
	return layerFound;
}

bool AreaTMX::processLayerData(XMLNode node, int z)
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

			if (gid < 0 || (int)gids.size() <= gid) {
				Log::err(descriptor, "invalid tile gid");
				return false;
			}

			// A gid of zero means there is no tile at this
			// position on this layer.
			if (gid > 0) {
				TileType* type = gids[gid];
				Tile& tile = map[z][y][x];
				type->allOfType.push_back(&tile);
				tile.parent = type;
			}

			if (++x == dim.x) {
				x = 0;
				y++;
			}
		}
	}

	return true;
}

bool AreaTMX::processObjectGroup(XMLNode node)
{

/*
 <objectgroup name="Prop0" width="5" height="5">
  <properties>
   <property name="layer" value="0.0"/>
  </properties>
  <object name="tile2" gid="7" x="64" y="320">
   <properties>
    <property name="onEnter" value="speed(0.5)"/>
    <property name="onLeave" value="undo()"/>
    <property name="onUse" value="undo()"/>
    <property name="exit" value="grassfield.area,1,1,0"/>
    <property name="flags" value="npc_nowalk"/>
   </properties>
  </object>
 </objectgroup>
*/

	double invalid = (double)NAN; // Not a number.
	int x, y;
	ASSERT(node.intAttr("width", &x));
	ASSERT(node.intAttr("height", &y));

	double depth = invalid;

	if (dim.x != x || dim.y != y) {
		Log::err(descriptor, "objectgroup x,y size != map x,y size");
		return false;
	}

	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		if (child.is("properties")) {
			ASSERT(processObjectGroupProperties(child, &depth));
		}
		else if (child.is("object")) {
			ASSERT(depth != invalid);
			int z = depth2idx[depth];
			ASSERT(processObject(child, z));
		}
	}

	return true;
}

bool AreaTMX::processObjectGroupProperties(XMLNode node, double* depth)
{

/*
  <properties>
   <property name="layer" value="0.0"/>
  </properties>
*/
	bool layerFound = false;

	for (XMLNode child = node.childrenNode(); child; child = child.next()) {
		std::string name = child.attr("name");
		std::string value = child.attr("value");
		if (name == "layer") {
			layerFound = true;
			ASSERT(child.doubleAttr("value", depth));
			if (depth2idx.find(*depth) == depth2idx.end()) {
				allocateMapLayer();
				depth2idx[*depth] = dim.z - 1;
				idx2depth.push_back(*depth);
				// Effectively idx2depth[dim.z - 1] = depth;
			}
		}
	}

	if (!layerFound)
		Log::err(descriptor, "<objectgroup> must have layer property");
	return layerFound;
}

bool AreaTMX::processObject(XMLNode node, int z)
{

/*
  <object name="tile2" gid="7" x="64" y="320">
   <properties>
    <property name="onEnter" value="speed(0.5)"/>
    <property name="onLeave" value="undo()"/>
    <property name="onUse" value="undo()"/>
    <property name="exit" value="grassfield.area,1,1,0"/>
    <property name="flags" value="npc_nowalk"/>
   </properties>
  </object>
  <object name="foo" x="0" y="0" width="64" height="64">
   ...
  </object>
*/

	// Gather object properties now. Assign them to tiles later.
	bool wwide[5], hwide[5]; /* wide exit in dimensions: width, height */

	ScriptInst enterScript, leaveScript, useScript;
	boost::scoped_ptr<Exit> exit[5];
	boost::optional<double> layermods[5];
	unsigned flags = 0x0;

	XMLNode child = node.childrenNode(); // <properties>
	if (!child) {
		// Empty <object> element. Odd, but acceptable.
		return true;
	}
	for (child = child.childrenNode(); child; child = child.next()) {
		// Each <property>...
		std::string name = child.attr("name");
		std::string value = child.attr("value");
		if (name == "flags") {
			ASSERT(splitTileFlags(value, &flags));
		}
		else if (name == "on_enter") {
			std::string filename = value;
			ScriptInst script(filename);
			if (!script.validate(descriptor))
				return false;
			enterScript = script;
		}
		else if (name == "on_leave") {
			std::string filename = value;
			ScriptInst script(filename);
			if (!script.validate(descriptor))
				return false;
			leaveScript = script;
		}
		else if (name == "on_use") {
			std::string filename = value;
			ScriptInst script(filename);
			if (!script.validate(descriptor))
				return false;
			useScript = script;
		}
		else if (name == "exit") {
			exit[EXIT_NORMAL].reset(new Exit);
			ASSERT(parseExit(value, exit[EXIT_NORMAL].get(), &wwide[EXIT_NORMAL], &hwide[EXIT_NORMAL]));
			flags |= TILE_NOWALK_NPC;
		}
		else if (name == "exit:up") {
			exit[EXIT_UP].reset(new Exit);
			ASSERT(parseExit(value, exit[EXIT_UP].get(), &wwide[EXIT_UP], &hwide[EXIT_UP]));
		}
		else if (name == "exit:down") {
			exit[EXIT_DOWN].reset(new Exit);
			ASSERT(parseExit(value, exit[EXIT_DOWN].get(), &wwide[EXIT_DOWN], &hwide[EXIT_DOWN]));
		}
		else if (name == "exit:left") {
			exit[EXIT_LEFT].reset(new Exit);
			ASSERT(parseExit(value, exit[EXIT_LEFT].get(), &wwide[EXIT_LEFT], &hwide[EXIT_LEFT]));
		}
		else if (name == "exit:right") {
			exit[EXIT_RIGHT].reset(new Exit);
			ASSERT(parseExit(value, exit[EXIT_RIGHT].get(), &wwide[EXIT_RIGHT], &hwide[EXIT_RIGHT]));
		}
		else if (name == "layermod") {
			double mod;
			ASSERT(child.doubleAttr("value", &mod));
			layermods[EXIT_NORMAL].reset(mod);
			flags |= TILE_NOWALK_NPC;
		}
		else if (name == "layermod:up") {
			double mod;
			ASSERT(child.doubleAttr("value", &mod));
			layermods[EXIT_UP].reset(mod);
		}
		else if (name == "layermod:down") {
			double mod;
			ASSERT(child.doubleAttr("value", &mod));
			layermods[EXIT_DOWN].reset(mod);
		}
		else if (name == "layermod:left") {
			double mod;
			ASSERT(child.doubleAttr("value", &mod));
			layermods[EXIT_LEFT].reset(mod);
		}
		else if (name == "layermod:right") {
			double mod;
			ASSERT(child.doubleAttr("value", &mod));
			layermods[EXIT_RIGHT].reset(mod);
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

		// We don't actually use the object gid. It is supposed to
		// indicate which tile our object is rendered as, but for
		// Tsunagari, tile objects are always transparent and reveal
		// the tile below.
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
			Tile& tile = map[z][Y][X];

			tile.flags |= flags;
			for (int i = 0; i < 5; i++) {
				if (exit[i]) {
					tile.exits[i] = new Exit(*exit[i].get());
					int dx = X - x;
					int dy = Y - y;
					if (wwide[i])
						tile.exits[i]->coords.x += dx;
					if (hwide[i])
						tile.exits[i]->coords.y += dy;
				}
			}
			for (int i = 0; i < 5; i++)
				if (layermods[i])
					tile.layermods[i] = layermods[i];
			tile.enterScript = enterScript;
			tile.leaveScript = leaveScript;
			tile.useScript = useScript;
		}
	}

	return true;
}

bool AreaTMX::splitTileFlags(const std::string& strOfFlags, unsigned* flags)
{
	std::vector<std::string> strs = splitStr(strOfFlags, ",");

	BOOST_FOREACH(const std::string& str, strs) {
		if (str == "nowalk")
			*flags |= TILE_NOWALK;
		else {
			Log::err(descriptor, "invalid tile flag: " + str);
			return false;
		}
	}
	return true;
}

/**
 * Matches regex /\s*\d+\+?/
 */
static bool isIntegerOrPlus(const std::string& s)
{
	const int space = 0;
	const int digit = 1;
	const int sign = 2;

	int state = space;

	for (size_t i = 0; i < s.size(); i++) {
		char c = s[i];
		if (state == space) {
		       if (isspace(c)) continue;
		       else state++;
		}
		if (state == digit) {
			if (isdigit(c)) continue;
			else state++;
		}
		if (state == sign) {
			if (c == '+') return true;
			else return false;
		}
	}
	return true;
}

bool AreaTMX::parseExit(const std::string& dest, Exit* exit,
	bool* wwide, bool* hwide)
{

/*
  Format: destination area, x, y, z
  E.g.:   "babysfirst.area,1,3,0"
*/

	std::vector<std::string> strs = splitStr(dest, ",");

	if (strs.size() != 4) {
		Log::err(descriptor, "<exit />: invalid format");
		return false;
	}

	std::string area = strs[0],
	            xstr = strs[1],
	            ystr = strs[2],
		    zstr = strs[3];

	if (!isIntegerOrPlus(xstr) ||
	    !isIntegerOrPlus(ystr) ||
	    !isIntegerOrPlus(zstr)) {
		Log::err(descriptor, "<exit />: invalid format");
		return false;
	}

	exit->area = area;
	exit->coords.x = atoi(xstr.c_str());
	exit->coords.y = atoi(ystr.c_str());
	exit->coords.z = atof(zstr.c_str());

	*wwide = xstr.find('+') != std::string::npos;
	*hwide = ystr.find('+') != std::string::npos;

	return true;
}

bool AreaTMX::parseRGBA(const std::string& str,
	Gosu::Color::Channel* r,
	Gosu::Color::Channel* g,
	Gosu::Color::Channel* b,
	Gosu::Color::Channel* a)
{
	std::vector<std::string> strs = splitStr(str, ",");

	if (strs.size() != 4) {
		Log::err(descriptor, "invalid RGBA format");
		return false;
	}

	Gosu::Color::Channel* channels[] = { r, g, b, a };

	for (int i = 0; i < 4; i++) {
		std::string s = strs[i];
		if (!isInteger(s)) {
			Log::err(descriptor, "invalid RGBA format");
			return false;
		}
		int v = atoi(s.c_str());
		if (!(0 <= v && v < 256)) {
			Log::err(descriptor,
				"RGBA values must be between 0 and 255");
			return false;
		}
		*channels[i] = (Gosu::Color::Channel)v;
	}

	return true;
}

