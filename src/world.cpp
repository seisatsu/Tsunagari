/******************************
** Tsunagari Tile Engine     **
** world.cpp                 **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Graphics.hpp> // for Gosu::Transform
#include <Gosu/Utility.hpp>

#include "area.h"
#include "common.h"
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
	: rc(rc), wnd(wnd), conf(conf), view(*wnd, *conf), area(NULL),
	  player(rc, NULL, conf)
{
	globalWorld = this;
}

World::~World()
{
	delete area;
	delete music;
}

bool World::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return false;

	music = new Music(rc);

	if (!player.init(xml.playerentity))
		return false;
	player.setPhase("down");
	view.trackEntity(&player);

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
	Gosu::Graphics& graphics = wnd->graphics();
	rvec2 off = view.getOffset();
	rvec2 scale = view.getScale();
	Gosu::Transform t = { {
		scale.x,          0,                0, 0,
		0,                scale.y,          0, 0,
		0,                0,                1, 0,
		scale.x * -off.x, scale.y * -off.y, 0, 1
	} };

	graphics.pushTransform(t);
	area->draw();
	graphics.popTransform();
}

bool World::needsRedraw() const
{
	return area->needsRedraw();
}

void World::update(unsigned long dt)
{
	area->update(dt);
}

bool World::loadArea(const std::string& areaName, icoord playerPos)
{
	Area* oldArea = area;
	Area* newArea = new Area(rc, this, &view, &player, music, areaName);
	if (!newArea->init()) {
		delete newArea;
		return false;
	}
	setArea(newArea, playerPos);
	delete oldArea;
	return true;
}

void World::setArea(Area* area, icoord playerPos)
{
	this->area = area;
	player.setArea(area);
	player.setTileCoords(playerPos);
	view.setArea(area);
}

bool World::processDescriptor()
{
	XMLRef doc = rc->getXMLDoc("world.conf", "world.dtd");
	if (!doc)
		return false;
	const XMLNode root = doc->root(); // <world>
	if (!root)
		return false;

	for (XMLNode node = root.childrenNode(); node; node = node.next()) {
		if (node.is("name")) {
			xml.name = node.content();
		} else if (node.is("author")) {
			xml.author = node.content();
		} else if (node.is("player")) {
			xml.playerentity = node.content();
		} else if (node.is("type")) {
			std::string str = node.attr("locality");
			if (str == "local")
				xml.locality = LOCAL;
			else if (str == "network")
				xml.locality = NETWORK;

			str = node.attr("movement");
			if (str == "turn")
				conf->moveMode = TURN;
			else if (str == "tile")
				conf->moveMode = TILE;
			else if (str == "notile")
				conf->moveMode = NOTILE;
		} else if (node.is("entrypoint")) {
			xml.entry.area = node.attr("area");
			if (!node.intAttr("x", &xml.entry.coords.x) ||
			    !node.intAttr("y", &xml.entry.coords.y) ||
			    !node.intAttr("z", &xml.entry.coords.z))
				return false;
		} else if (node.is("initscript")) {
			xml.initscript = node.content();
		}
	}
	return true;
}

