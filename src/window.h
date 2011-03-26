/****************************
** Tsunagari Tile Engine   **
** window.h - Window       **
** Copyright 2011 OmegaSDG **
****************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <Gosu/Gosu.hpp>

#include "entity.h"
//#include "resourcer.h"
//#include "world.h"

class Entity;

class GameWindow : public Gosu::Window
{
public:
	GameWindow();
	void button_down(int id);
	void draw();
	void update();
	bool needs_redraw();

private:
//	string get_datadir();
	Entity* player;
};

#endif

