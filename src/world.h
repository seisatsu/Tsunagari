/******************************
** Tsunagari Tile Engine     **
** world.h - World           **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef WORLD_H
#define WORLD_H

#include <Gosu/Gosu.hpp>

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
	~World();

	void button_down(Gosu::Button btn);
	void draw();
	bool needs_redraw();

/*
	void button_down(int id);
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

