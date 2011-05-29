/******************************
** Tsunagari Tile Engine     **
** world.cpp                 **
** Copyright 2011 OmegaSDG   **
******************************/

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "area.h"
#include "entity.h"
#include "log.h"
#include "resourcer.h"
#include "window.h"
#include "world.h"

World::World(Resourcer* rc, GameWindow* wnd)
	: rc(rc), wnd(wnd), area(NULL), player(NULL)
{
}

World::~World()
{
	delete player;
	delete area;
}

bool World::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return false;

	// The player entity doesn't have a descriptor yet.
	player = new Entity(rc, "_NONE_", xml.playersprite);
	if (!player->init())
		return false;

	area = new Area(rc, player, xml.entry.area);
	
	wnd->setCaption(Gosu::widen(xml.name));
	return area->init();
}

void World::buttonDown(const Gosu::Button btn)
{
	area->buttonDown(btn);
}

void World::draw()
{
	area->draw();
}

bool World::needsRedraw() const
{
	return area->needsRedraw();
}

bool World::processDescriptor()
{
	static const std::string descriptor = "world.conf";
	xmlChar* str;
	
	const xmlNode* root = rc->getXMLDoc(descriptor);
	if (!root)
		return false;
	
	xmlNode* node = root->xmlChildrenNode; // <world>
	node = node->xmlChildrenNode; // decend into children of <world>
	while (node != NULL) {
		if (!xmlStrncmp(node->name, BAD_CAST("name"), 5)) {
			str = xmlNodeGetContent(node);
			xml.name = (char*)str;
		}
		if (!xmlStrncmp(node->name, BAD_CAST("author"), 7)) {
			str = xmlNodeGetContent(node);
			xml.author = (char*)str;
		}
		if (!xmlStrncmp(node->name, BAD_CAST("type"), 5)) {
			str = xmlNodeGetContent(node);
			
			if (xmlStrncmp(str, BAD_CAST("local"), 6))
				xml.type = LOCAL;
			
			if (xmlStrncmp(str, BAD_CAST("network"), 8))
				xml.type = NETWORK;
			
			else {
				Log::err(descriptor, "Invalid <type> value");
				return false;
			}
		}
		if (!xmlStrncmp(node->name, BAD_CAST("player"), 7)) {
			str = xmlGetProp(node, BAD_CAST("sprite"));
			xml.playersprite = (char*)str;
		}
		if (!xmlStrncmp(node->name, BAD_CAST("entrypoint"), 11)) {
			str = xmlGetProp(node, BAD_CAST("area"));
			xml.entry.area = (char*)str;
			
			str = xmlGetProp(node, BAD_CAST("x"));
			xml.entry.coords.x = atol((char*)str);
			
			str = xmlGetProp(node, BAD_CAST("y"));
			xml.entry.coords.y = atol((char*)str);
			
			str = xmlGetProp(node, BAD_CAST("z"));
			xml.entry.coords.z = atol((char*)str);
		}
		node = node->next;
	}
	return true;
}

