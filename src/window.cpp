/******************************
** Tsunagari Tile Engine     **
** window.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include "resourcer.h"
#include "world.h"
#include "window.h"

// === Roguelike Input Mode Settings ===
	// Milliseconds of button down before starting persistent input.
	#define ROGUELIKE_PERSIST_DELAY_INIT 500
	
	// Milliseconds between persistent input sends.
	#define ROGUELIKE_PERSIST_DELAY_CONSECUTIVE 100
// ===

static GameWindow* globalWindow = NULL;

GameWindow* GameWindow::getWindow()
{
	return globalWindow;
}

GameWindow::GameWindow(uint x, uint y, bool fullscreen)
	: Gosu::Window(x, y, fullscreen)
{
	globalWindow = this;
}

GameWindow::~GameWindow()
{
	delete world;
	delete rc;
}

bool GameWindow::init(const std::string descriptor)
{
	rc = new Resourcer(this, descriptor);
	world = new World(rc, this);
	return rc->init() && world->init();
}

void GameWindow::buttonDown(const Gosu::Button btn)
{
	if (btn == Gosu::kbEscape)
		close();
	else {
		if (keystates.find(btn) == keystates.end())
			keystates[btn].second = Gosu::milliseconds();
		world->buttonDown(btn);
	}
}

void GameWindow::buttonUp(const Gosu::Button btn)
{
	if (keystates.find(btn) != keystates.end())
		keystates.erase(btn);
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
	std::map<Gosu::Button, std::pair<bool, unsigned long> >::iterator it;
	
	// Persistent input handling code
	for (it = keystates.begin(); it != keystates.end(); it++) {
		if ((*it).second.first == false) {
			if (Gosu::milliseconds() >= 
			   (*it).second.second+ROGUELIKE_PERSIST_DELAY_INIT) {
			   	(*it).second.second = Gosu::milliseconds();
				(*it).second.first = true;
				world->buttonDown((*it).first);
			}
			else
				continue;
		}
		else {
			if (Gosu::milliseconds() >= 
			   (*it).second.second+ROGUELIKE_PERSIST_DELAY_CONSECUTIVE) {
			   	(*it).second.second = Gosu::milliseconds();
				world->buttonDown((*it).first);
			}
			else
				continue;
		}
	}
}

