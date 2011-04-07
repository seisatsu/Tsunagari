/******************************
** Tsunagari Tile Engine     **
** sprite.cpp - Sprite       **
** Copyright 2011 OmegaSDG   **
******************************/

#include "sprite.h"

Sprite::Sprite(Resourcer* rc, const std::string descriptor)
{
	this->rc = rc;
	this->descriptor = descriptor;
	values.phases = new SpritePhases;
	c.x = c.y = 0;
}

Sprite::~Sprite()
{
	delete img;
	delete values.phases;
}

bool Sprite::processDescriptor()
{
	std::ifstream file(descriptor.c_str());
	
	Json::Value root;
	Json::Value phases;
	Json::Reader reader;
	
	// Here we load in the sprite descriptor file. It's a little messy.
	if (!reader.parse(file, root)) // Actual parsing.
		return false;
	
	// Begin loading in configuration values.
	values.sheet = root.get("sheet", "_NONE_").asString(); // sheet
	if (values.sheet.compare("_NONE_") == 0) {
		std::cerr << "Error: " << descriptor << ": \"sheet\" required.\n";
		return false;
	}
	
	phases = root["phases"]; // phases
	if (phases.size() < 1) {
		std::cerr << "Error: " << descriptor << ": \"phases\" [>0] required.\n";
		return false;
	}
	values.phases->phase = phases.get("player", 0).asUInt();
	
	file.close();
	return true;
}

int Sprite::init()
{
	if (!processDescriptor()) // Try to load in descriptor.
		return 3;

	img = rc->getImage(values.sheet);
	
	return 0;
}

void Sprite::draw()
{
	img->draw(c.x, c.y, 0);
}

void Sprite::move(int dx, int dy)
{
	c.x += dx;
	c.y += dy;
}

