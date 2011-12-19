/******************************
** Tsunagari Tile Engine     **
** world.cpp                 **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Utility.hpp>

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
	: rc(rc), wnd(wnd), conf(conf), player(rc, NULL, conf)
{
	globalWorld = this;
}

World::~World()
{
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

	view = new Viewport(*wnd, xml.viewport);
	view->trackEntity(&player);

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

	drawLetterbox();
	graphics.pushTransform(getTransform());
	area->draw();
	graphics.popTransform();
}

bool World::needsRedraw() const
{
	return area->needsRedraw();
}

void World::update(unsigned long dt)
{
	// Prevent the Area from being deleted during this function call.
	// Otherwise if it accesses its member variables and we've overwritten
	// the memory, bad things happen.
	boost::shared_ptr<Area> safe(area);

	area->update(dt);
}

bool World::loadArea(const std::string& areaName, icoord playerPos)
{
	AreaPtr oldArea(area);
	AreaPtr newArea(new Area(rc, this, view, &player, music, areaName));
	if (!newArea->init())
		return false;
	setArea(newArea, playerPos);
	// oldArea is deleted
	return true;
}

void World::setArea(AreaPtr area, icoord playerPos)
{
	this->area = area;
	player.setArea(area.get());
	player.setTileCoords(playerPos);
	view->setArea(area.get());
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
		} else if (node.is("viewport")) {
			if (!node.intAttr("width", &xml.viewport.x) ||
			    !node.intAttr("height", &xml.viewport.y))
				return false;
		} else if (node.is("initscript")) {
			xml.initscript = node.content();
		}
	}
	return true;
}

void World::drawLetterbox()
{
	rvec2 sz = view->getPhysRes();
	rvec2 lb = rvec2(0.0, 0.0);
	lb -= view->getLetterboxOffset();
	Gosu::Color black = Gosu::Color::BLACK;

	drawRect(0, sz.x, 0, lb.y, black, 1000);
	drawRect(0, sz.x, sz.y - lb.y, sz.y, black, 1000);
	drawRect(0, lb.x, 0, sz.y, black, 1000);
	drawRect(sz.x - lb.x, sz.x, 0, sz.y, black, 1000);
}

void World::drawRect(double x1, double x2, double y1, double y2,
                       Gosu::Color c, double z)
{
	wnd->graphics().drawQuad(
		x1, y1, c,
		x2, y1, c,
		x2, y2, c,
		x1, y2, c,
		z
	);
}

Gosu::Transform World::getTransform()
{
	rvec2 scale = view->getScale();
	rvec2 scroll = view->getMapOffset();
	rvec2 padding = view->getLetterboxOffset();
	Gosu::Transform t = { {
		scale.x,          0,                0, 0,
		0,                scale.y,          0, 0,
		0,                0,                1, 0,
		scale.x * -scroll.x - padding.x, scale.y * -scroll.y - padding.y, 0, 1
	} };
	return t;
}

