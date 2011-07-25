/******************************
** Tsunagari Tile Engine     **
** world.cpp                 **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Utility.hpp>
#include <libxml/tree.h>

#include "area.h"
#include "player.h"
#include "log.h"
#include "resourcer.h"
#include "window.h"
#include "world.h"

static World* globalWorld = NULL;

World* World::getWorld()
{
	return globalWorld;
}

World::World(Resourcer* rc, GameWindow* wnd)
	: rc(rc), wnd(wnd), area(NULL)
{
	globalWorld = this;
}

World::~World()
{
	delete area;
}

bool World::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return false;

	// FIXME The player entity doesn't have a descriptor yet.
	player.reset(new Player(rc, NULL));
	if (!player->init(xml.playerentity))
		return false;
	player->setPhase("down");

	wnd->setCaption(Gosu::widen(xml.name));
	return loadArea(xml.entry.area, xml.entry.coords);
}

void World::buttonDown(const Gosu::Button btn)
{
	area->buttonDown(btn);
}

void World::buttonUp(const Gosu::Button btn)
{
	area->buttonUp(btn);
}

void World::draw()
{
	area->draw();
}

bool World::needsRedraw() const
{
	return area->needsRedraw();
}

void World::update(unsigned long dt)
{
	area->update(dt);
}

bool World::processDescriptor()
{
	static const std::string descriptor = "world.conf";
	xmlChar* str;
	
	XMLDocRef doc = rc->getXMLDoc(descriptor, "world.dtd");
	if (!doc)
		return false;
	const xmlNode* root = xmlDocGetRootElement(doc.get()); // <world>
	if (!root)
		return false;

	xmlNode* node = root->xmlChildrenNode; // children of <world>
	for (; node != NULL; node = node->next) {
		if (!xmlStrncmp(node->name, BAD_CAST("name"), 5)) {
			str = xmlNodeGetContent(node);
			xml.name = (char*)str;
		}
		if (!xmlStrncmp(node->name, BAD_CAST("author"), 7)) {
			str = xmlNodeGetContent(node);
			xml.author = (char*)str;
		}
		if (!xmlStrncmp(node->name, BAD_CAST("player"), 7)) {
			str = xmlNodeGetContent(node);
			xml.playerentity = (char*)str;
		}
		if (!xmlStrncmp(node->name, BAD_CAST("type"), 5)) {
			str = xmlGetProp(node, BAD_CAST("locality"));
			
			if (xmlStrncmp(str, BAD_CAST("local"), 6))
				xml.locality = LOCAL;
			
			else if (xmlStrncmp(str, BAD_CAST("network"), 8))
				xml.locality = NETWORK;

			else {
				Log::err(descriptor, "Invalid <type> value");
				return false;
			}
			
			str = xmlGetProp(node, BAD_CAST("movement"));
			
			if (xmlStrncmp(str, BAD_CAST("turn"), 5))
				xml.movement = TURN;
			
			else if (xmlStrncmp(str, BAD_CAST("tile"), 5))
				xml.movement = TILE;
			
			else if (xmlStrncmp(str, BAD_CAST("notile"), 7))
				xml.movement = NOTILE;
			
			else {
				Log::err(descriptor, "Invalid <locality> value");
				return false;
			}
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
		if (!xmlStrncmp(node->name, BAD_CAST("scripts"), 13)) {
			node = node->xmlChildrenNode; // decend
		}
		if (!xmlStrncmp(node->name, BAD_CAST("script"), 7)) {
			str = xmlNodeGetContent(node);
			xml.scripts.push_back((char*)str);
		}
	}
	return true;
}


bool World::loadArea(const std::string& areaName, coord_t playerPos)
{
	Area* newArea = new Area(rc, this, player.get(), areaName);
	delete area;
	area = newArea;
	if (!area->init())
		return false;
	player->setArea(area);
	player->setCoordsByTile(playerPos);
	return true;
}

