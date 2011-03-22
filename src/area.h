/****************************
** Tsunagari Tile Engine   **
** area.h - Area           **
** Copyright 2011 OmegaSDG **
****************************/

#include "player.h"
#include "resourcer.h"
#include "tilegrid.h"
#include "window.h"

class Area
{
public:
	Area(Window* window, Resourcer* rc, Entity* player, const char* filename);
	void button_down(int id);
	void draw();
	coord_t translate_coords();

private:
	void build(Resourcer* rc, yaml_t* yaml);
	Window* window;
	Entity* player;
	TileGrid* grid;
};

