/******************************
** Tsunagari Tile Engine     **
** window.h - Window         **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <string>

#include <Gosu/Gosu.hpp>

#include "resourcer.h"
#include "world.h"

class Entity;
class Resourcer;
class World;

class GameWindow : public Gosu::Window
{
public:
	GameWindow(uint x, uint y, bool fullscreen);
	virtual ~GameWindow();

	int init(const std::string descriptor);
	void buttonDown(const Gosu::Button btn);
	void draw();
	bool needsRedraw() const;
	void update();

private:
//	string getDataDir();
	Resourcer* rc;
	World* world;
};

#endif

