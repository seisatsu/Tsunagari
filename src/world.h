/******************************
** Tsunagari Tile Engine     **
** world.h - World           **
** Copyright 2011 OmegaSDG   **
******************************/

#include "area.h"
#include "entity.h"
#include "resourcer.h"
#include "window.h"

class World
{
public:
	World(Window* window, Resourcer* rc);
	void button_down(int id);
	void draw();
	bool needs_redraw();
	void load_area(Area* area);

private:
	Window* window;
	Resourcer* rc;
	Area* area;
	Entity* player;
	bool redraw;
};

