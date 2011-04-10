/******************************
** Tsunagari Tile Engine     **
** window.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <json/json.h>

#include "window.h"

GameWindow::GameWindow(uint x, uint y, bool fullscreen)
	: Gosu::Window(x, y, fullscreen), rc(new Resourcer(this))
{
}

GameWindow::~GameWindow()
{
	if (world)
		delete world;
}

bool GameWindow::init(const std::string descriptor)
{
	world = new World(this, rc, descriptor);
	return world->init();
}

void GameWindow::buttonDown(const Gosu::Button btn)
{
	if (btn == Gosu::kbEscape)
		close();
	else
		world->buttonDown(btn);
}

void GameWindow::draw()
{
	world->draw();
}

bool GameWindow::needsRedraw() const
{
	return world->needsRedraw();
}

void GameWindow::update()
{
}

