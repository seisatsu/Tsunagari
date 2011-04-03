/******************************
** Tsunagari Tile Engine     **
** world.h - World           **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef WORLD_H
#define WORLD_H

#include "area.h"
#include "entity.h"
#include "resourcer.h"
#include "window.h"

class Area;
class Entity;
class GameWindow;
class Resourcer;

class World
{
public:
	World(GameWindow* window, Resourcer* rc);
	void draw();

/*
	void button_down(int id);
	bool needs_redraw();
	void load_area(Area* area);
*/

private:
	Area* area;
	Entity* player;

/*
	Window* window;
	Resourcer* rc;
	bool redraw;
*/

};

#endif

