/******************************
** Tsunagari Tile Engine     **
** sprite.cpp - Sprite       **
** Copyright 2011 OmegaSDG   **
******************************/

#include <fstream>
#include <istream>

#include <json/json.h>

#include "log.h"
#include "sprite.h"

Sprite::Sprite(Resourcer* rc, const std::string descriptor)
	: rc(rc), img(NULL), descriptor(descriptor)
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
	if (!reader.parse(file, root)) {
		Log::err(descriptor, "File missing.");
		return false;
	}

	// Begin loading in configuration values.
	values.sheet = root["sheet"].asString();
	if (values.sheet.empty()) {
		Log::err(descriptor, "\"sheet\" required.\n");
		return false;
	}

	phases = root["phases"];
	if (!phases.size()) {
		Log::err(descriptor, "\"phases\" [>0] required.\n");
		return false;
	}
	values.phases.phase = phases.get("player", 0).asUInt();

	return true;
}

bool Sprite::init()
{
	if (!processDescriptor())
		return false;

	img = rc->getImage(values.sheet);

	return true;
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

