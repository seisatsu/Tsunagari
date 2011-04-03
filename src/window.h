/******************************
** Tsunagari Tile Engine     **
** window.h - Window         **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <Gosu/Gosu.hpp>

//#include "resourcer.h"
#include "world.h"

class Entity;
class World;

class GameWindow : public Gosu::Window
{
public:
	GameWindow();
	virtual ~GameWindow();

	void buttonDown(Gosu::Button btn);
	void draw();
	bool needsRedraw();
	void update();

private:
//	string get_datadir();
	World* world;
};

#endif

