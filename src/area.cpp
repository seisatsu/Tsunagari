/******************************
** Tsunagari Tile Engine     **
** area.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <boost/shared_ptr.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "area.h"
#include "common.h"
#include "entity.h"
#include "log.h"
#include "resourcer.h"
#include "sprite.h"

Area::Area(Resourcer* rc, Entity* player, const std::string descriptor)
	: rc(rc), player(player), descriptor(descriptor)
{
	dim.z = 0;
}

Area::~Area()
{
}

bool Area::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return false;
	return true;
}

void Area::buttonDown(const Gosu::Button btn)
{
	if (btn == Gosu::kbRight)
		player->moveByTile(coord(1, 0, 0));
	else if (btn == Gosu::kbLeft)
		player->moveByTile(coord(-1, 0, 0));
	else if (btn == Gosu::kbUp)
		player->moveByTile(coord(0, -1, 0));
	else if (btn == Gosu::kbDown)
		player->moveByTile(coord(0, 1, 0));
}

void Area::draw()
{
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
			unsigned id = atol((const char*)idstr);

			// Undeclared TileTypes have default properties.
			while (ts.defaults.size() != id) {
				TileType tt = defaultTileType(ts.source,
					ts.tiledim, ts.defaults.size());
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
	unsigned tilesz = ts.tiledim.x * ts.tiledim.y;
	while (ts.defaults.size() != srcsz / tilesz) {
		TileType tt = defaultTileType(ts.source,
			ts.tiledim, ts.defaults.size());
		ts.defaults.push_back(tt);
	}

	tilesets.push_back(ts);
	return true;
}

Area::TileType Area::defaultTileType(const Gosu::Bitmap source, coord_t tiledim,
                               int id)
{
	int x = (tiledim.x * (id-1)) % source.width();
	int y = (tiledim.y * id) / source.height() * tiledim.y; // ???
	
	TileType tt;
	Gosu::Image* img = rc->bitmapSection(source, x, y,
	                                     tiledim.x, tiledim.y, true);
	tt.graphics.push_back(img);
	tt.animated = false;
	tt.ani_speed = 0.0;
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
		ts.tiledim, ts.defaults.size());

	xmlChar* idstr = xmlGetProp(node, BAD_CAST("id"));
	unsigned id = atol((const char*)idstr);
	if (id != ts.defaults.size()) {
		// XXX we need to know the Area we're loading...
		Log::err("unknown area", std::string("expected TileType id ") +
		         itostr(ts.defaults.size()) + ", but got " + itostr(id));
		return false;
	}

	xmlNode* child = node->xmlChildrenNode; // <properties>
	child = node->xmlChildrenNode; // <property>
	for (; child != NULL; child = child->next) {
		xmlChar* name = xmlGetProp(child, BAD_CAST("name"));
		xmlChar* value = xmlGetProp(child, BAD_CAST("value"));
		if (!xmlStrncmp(child->name, BAD_CAST("flags"), 6)) {
			// TODO flags
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
			tt.ani_speed = atol((const char*)value);
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
	unsigned x = atol((const char*)width);
	unsigned y = atol((const char*)height);

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
			unsigned depth = atol((const char*)value);
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
			unsigned gid = atol((const char*)gidStr);
			Tile* t = new Tile;
			t->type = &tilesets[0].defaults[gid]; // XXX can only access first tileset
			row.push_back(t);
			if (i % dim.x == 0) {
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
	unsigned x = atol((const char*)width);
	unsigned y = atol((const char*)height);

	unsigned zpos = -1;

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
			if (zpos == (unsigned)-1 || !processObject(child, zpos))
				return false;
		}
	}

	return true;
}

bool Area::processObjectGroupProperties(xmlNode* node, unsigned* zpos)
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
			int layer = atol((const char*)value);
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

bool Area::processObject(xmlNode* node, unsigned zpos)
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
	unsigned x = atol((const char*)xStr) / tilesets[0].tiledim.x;
	unsigned y = atol((const char*)yStr) / tilesets[0].tiledim.y;
	y = y - 1; // bug in tiled? y is 1 too high

	// We know which Tile is being talked about now... yay
	Tile* t = map[zpos][y][x];

	xmlNode* child = node->xmlChildrenNode; // <properties>
	child = node->xmlChildrenNode; // <property>
	for (; child != NULL; child = child->next) {
		xmlChar* name = xmlGetProp(child, BAD_CAST("name"));
		xmlChar* value = xmlGetProp(child, BAD_CAST("value"));
		if (!xmlStrncmp(child->name, BAD_CAST("flags"), 6)) {
			// TODO flags
		}
		else if (!xmlStrncmp(name, BAD_CAST("onEnter"), 8)) {
			// TODO events
		}
		else if (!xmlStrncmp(name, BAD_CAST("onLeave"), 8)) {
			// TODO events
		}
		else if (!xmlStrncmp(name, BAD_CAST("door"), 5)) {
			// TODO doors
		}
	}
	return true;
}

coord_t Area::getDimensions() const
{
	return dim;
}

Area::Tile* Area::getTile(coord_t c)
{
	return map[c.z][c.y][c.x];
}

