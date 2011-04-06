/******************************
** Tsunagari Tile Engine     **
** area.h - Area             **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_H
#define AREA_H

#include <string>
#include <Gosu/Gosu.hpp>

#include "common.h"
#include "entity.h"
#include "resourcer.h"
//#include "tilegrid.h"
#include "window.h"

class Entity;
class GameWindow;
class Resourcer;

class Area
{
public:
	Area(GameWindow* window, Resourcer* rc, Entity* player, std::string filename);
	~Area();

	void button_down(Gosu::Button btn);
	void draw();
	bool needs_redraw();
	coord_t translate_coords();

private:
//	void build(Resourcer* rc, yaml_t* yaml);
//	GameWindow* window;
	Entity* player;
//	TileGrid* grid;
};

#endif

