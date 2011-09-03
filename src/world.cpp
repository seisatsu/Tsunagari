/******************************
** Tsunagari Tile Engine     **
** world.cpp                 **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Utility.hpp>
#include <libxml/tree.h>

#include "area.h"
#include "common.h"
#include "player.h"
#include "log.h"
#include "resourcer.h"
#include "window.h"
#include "world.h"
#include "xml.h"

static World* globalWorld = NULL;

World* World::getWorld()
{
	return globalWorld;
}

World::World(GameWindow* wnd, Resourcer* rc, ClientValues* conf)
	: rc(rc), wnd(wnd), conf(conf), area(NULL)
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
	player.reset(new Player(rc, NULL, conf));
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
	std::string str;
	
	XMLDoc doc = rc->getXMLDoc(descriptor, "world.dtd");
	if (!doc)
		return false;
	const xmlNode* root = doc.temporaryGetRoot(); // <world>
	if (!root)
		return false;

	xmlNode* node = root->xmlChildrenNode; // children of <world>
	for (; node != NULL; node = node->next) {
		if (!xmlStrncmp(node->name, BAD_CAST("name"), 5)) {
			xml.name = readXmlElement(node);
		}
		if (!xmlStrncmp(node->name, BAD_CAST("author"), 7)) {
			xml.author = readXmlElement(node);
		}
		if (!xmlStrncmp(node->name, BAD_CAST("player"), 7)) {
			xml.playerentity = readXmlElement(node);
		}
		if (!xmlStrncmp(node->name, BAD_CAST("type"), 5)) {
			str = readXmlAttribute(node, "locality");
			
			if (!str.compare("local"))
				xml.locality = LOCAL;
			
			else if (!str.compare("network"))
				xml.locality = NETWORK;

			else {
				Log::err(descriptor, "Invalid <locality> value");
				return false;
			}
			
			str = readXmlAttribute(node, "movement");
			
			if (!str.compare("turn"))
				conf->moveMode = TURN;
			
			else if (!str.compare("tile"))
				conf->moveMode = TILE;
			
			else if (!str.compare("notile"))
				conf->moveMode = NOTILE;
			
			else {
				Log::err(descriptor,
					"Invalid <movement> value");
				return false;
			}
		}
		if (!xmlStrncmp(node->name, BAD_CAST("entrypoint"), 11)) {
			xml.entry.area = readXmlAttribute(node, "area");
			
			str = readXmlAttribute(node, "x");
			xml.entry.coords.x = atoi(str.c_str());
			
			str = readXmlAttribute(node, "y");
			xml.entry.coords.y = atoi(str.c_str());
			
			str = readXmlAttribute(node, "z");
			xml.entry.coords.z = atoi(str.c_str());
		}
		if (!xmlStrncmp(node->name, BAD_CAST("scripts"), 13)) {
			node = node->xmlChildrenNode; // decend
		}
		if (!xmlStrncmp(node->name, BAD_CAST("script"), 7)) {
			str = readXmlElement(node);
			xml.scripts.push_back(str);
		}
	}
	return true;
}


bool World::loadArea(const std::string& areaName, icoord_t playerPos)
{
	Area* newArea = new Area(rc, this, player.get(), areaName);
	delete area;
	area = newArea;
	if (!area->init())
		return false;
	player->setArea(area);
	player->setTileCoords(playerPos);
	return true;
}

