/******************************
** Tsunagari Tile Engine     **
** window.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include "resourcer.h"
#include "window.h"

GameWindow::GameWindow() : Gosu::Window(640, 480, false)
{
	Resourcer rc(this);
	world = new World(this, &rc);
}

GameWindow::~GameWindow()
{
	delete world;
}

void GameWindow::buttonDown(Gosu::Button btn)
{
	if (btn == Gosu::kbEscape)
		close();
	else
		world->button_down(btn);
}

void GameWindow::draw()
{
	world->draw();
}

bool GameWindow::needsRedraw()
{
	return world->needs_redraw();
}

void GameWindow::update()
{
}

