/******************************
** Tsunagari Tile Engine     **
** sprite.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <fstream>
#include <istream>
#include <stdio.h>

#include <json/json.h>

#include "log.h"
#include "resourcer.h"
#include "sprite.h"

Sprite::Sprite(Resourcer* rc, const std::string descriptor)
	: rc(rc), img(NULL), descriptor(descriptor)
{
	c.x = c.y = 0;
}

Sprite::~Sprite()
{
	delete img;
}

/**
 * Try to load in descriptor.
 */
bool Sprite::processDescriptor()
{
	const Json::Value root = rc->getDescriptor(descriptor);
	if (root.empty())
		return false;

	// Begin loading in configuration values.
	values.sheet = root["sheet"].asString();
	if (values.sheet.empty()) {
		Log::err(descriptor, "\"sheet\" required.\n");
		return false;
	}
	
	if (root["tilesize"].size() != 2) {
		Log::err(descriptor, "\"tilesize\" [2] required.\n");
		return false;
	}
	
	values.tilesize.x = root["tilesize"][uint(0)].asUInt();
	values.tilesize.y = root["tilesize"][1].asUInt();

	const Json::Value phases = root["phases"];
	if (!phases.size()) {
		Log::err(descriptor, "\"phases\" [>0] required.\n");
		return false;
	}
	
	for (Json::ValueConstIterator i = phases.begin(); i != phases.end(); i++) {
		const std::string key = i.key().asString();
		const uint32_t value = (*i).asUInt();
		values.phases[key] = value;
	}

	for (std::map<std::string, uint32_t>::const_iterator
			i = values.phases.begin(); i != values.phases.end(); i++)
		printf("phase [%s] = index %d\n", i->first.c_str(), i->second);

	return true;
}

bool Sprite::init()
{
	if (!processDescriptor())
		return false;
	img = rc->getImage(values.sheet);
	return img != NULL;
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

