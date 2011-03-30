/****************************
** Tsunagari Tile Engine   **
** window.cpp              **
** Copyright 2011 OmegaSDG **
****************************/

#include "resourcer.h"
#include "window.h"
#include "entity.h"

GameWindow::GameWindow() : Gosu::Window(640, 480, false)
{
	Resourcer rc(this);
	player = new Entity(&rc, L"../testing/outpost_world/player.image");
}

void GameWindow::update()
{
}

void GameWindow::draw()
{
	player->draw();
}

