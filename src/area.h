/******************************
** Tsunagari Tile Engine     **
** area.h - Area             **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef AREA_H
#define AREA_H

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
	Area(GameWindow* window, Resourcer* rc, Entity* player, const char* filename);
	~Area();

	void button_down(int id);
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

