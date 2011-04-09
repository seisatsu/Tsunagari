/******************************
** Tsunagari Tile Engine     **
** sprite.cpp - Sprite       **
** Copyright 2011 OmegaSDG   **
******************************/

#include <fstream>
#include <iostream>

#include <json/json.h>

#include "messagehandler.h"
#include "sprite.h"

#define MSG() MessageHandler::console()

Sprite::Sprite(Resourcer* rc, const std::string descriptor)
	: rc(rc), descriptor(descriptor)
{
	c.x = c.y = 0;
}

Sprite::~Sprite()
{
	if (img)
		delete img;
}

/**
 * Try to load in descriptor.
 */
bool Sprite::processDescriptor()
{
	Json::Reader reader;
	Json::Value root;
	Json::Value phases;

	std::ifstream file(descriptor.c_str());

	// Here we load in the sprite descriptor file. It's a little messy.
	if (!reader.parse(file, root))
		return false;

	// Begin loading in configuration values.
	values.sheet = root["sheet"].asString();
	if (values.sheet.empty()) {
		MSG()->send(ERR, descriptor, "\"sheet\" required.\n");
		return false;
	}

	phases = root["phases"];
	if (!phases.size()) {
		MSG()->send(ERR, descriptor, "\"phases\" [>0] required.\n");
		return false;
	}
	values.phases.phase = phases.get("player", 0).asUInt();

	return true;
}

int Sprite::init()
{
	if (!processDescriptor())
		return 3;

	img = rc->getImage(values.sheet);

	return 0;
}

void Sprite::draw() const
{
	img->draw(c.x, c.y, 0);
}

void Sprite::move(int dx, int dy)
{
	c.x += dx;
	c.y += dy;
}

