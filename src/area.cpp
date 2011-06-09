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
#include "resourcer.h"
#include "sprite.h"

Area::Area(Resourcer* rc, Entity* player, const std::string descriptor)
	: rc(rc), player(player), descriptor(descriptor)
{
}

Area::~Area()
{
}

bool Area::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return false;
	
	Gosu::Image* grassImg = rc->getImage("grass.sheet");
	TileType* grassTile = new TileType;
	grassTile->graphics.push_back(grassImg);
	grassTile->animated = false;
	Tile* t = new Tile;
	Tile* t2 = new Tile;
	t->type = t2->type = grassTile;

	map.resize(1);
	map[0].resize(1);
	map[0][0].resize(2);
	map[0][0][0] = t;
	map[0][0][1] = t2;
	
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
			row_t row = grid[layer];
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
			while (ts.defaults.size() != id) {
				TileType tt = defaultTileType(ts.source,
					ts.tiledim, ts.defaults.size());
				ts.defaults.push_back(tt);
			}
			if (!processTileType(child))
				return false;
		}
		else if (!xmlStrncmp(child->name, BAD_CAST("image"), 6)) {
			xmlChar* source = xmlGetProp(child, BAD_CAST("source"));
			ts.source = rc->getBitmap((const char*)source);
		}
	}
	return true;
}

Area::TileType Area::defaultTileType(const Gosu::Bitmap source, coord_t tiledim,
                               int id)
{
	int x = tiledim.x * (id % source.width());
	int y = tiledim.y * (id / source.height());
	
	TileType tt;
	Gosu::Image* img = rc->bitmapSection(source, x, y,
	                                     tiledim.x, tiledim.y, true);
	tt.graphics.push_back(img);
	tt.animated = false;
	tt.ani_speed = 0.0;
	return tt;
}

bool Area::processTileType(xmlNode* node)
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
	
	return true;
}

bool Area::processLayer(xmlNode* node)
{
	return true;
}

bool Area::processObjectGroup(xmlNode* node)
{
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

