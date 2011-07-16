/******************************
** Tsunagari Tile Engine     **
** window.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Timing.hpp>
#include <Gosu/Utility.hpp>

#include "config.h"
#include "resourcer.h"
#include "world.h"
#include "window.h"

static GameWindow* globalWindow = NULL;

GameWindow* GameWindow::getWindow()
{
	return globalWindow;
}

GameWindow::GameWindow(unsigned x, unsigned y, bool fullscreen)
	: Gosu::Window(x, y, fullscreen),
	  lastTime(Gosu::milliseconds())
{
	globalWindow = this;
}

GameWindow::~GameWindow()
{
	delete world;
	delete rc;
}

bool GameWindow::init(ClientValues* conf)
{
	rc = new Resourcer(this, conf);
	world = new World(rc, this);
	return rc->init() && world->init();
}

void GameWindow::buttonDown(const Gosu::Button btn)
{
	if (btn == Gosu::kbEscape)
		close();
	else {
		if (keystates.find(btn) == keystates.end()) {
			keystate& state = keystates[btn];
			state.since = Gosu::milliseconds();
			state.initiallyResolved = false;
			state.consecutive = false;

			// We process the initial buttonDown here so that it
			// gets handled even if we receive a buttonUp before an
			// update.
			world->buttonDown(btn);
		}
	}
}

void GameWindow::buttonUp(const Gosu::Button btn)
{
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
	calculateDt();
	handleKeyboardInput();
	world->update(dt);
}

void GameWindow::calculateDt()
{
	unsigned long now = Gosu::milliseconds();
	dt = now - lastTime;
	lastTime = now;
}

void GameWindow::handleKeyboardInput()
{
	std::map<Gosu::Button, keystate>::iterator it;
	unsigned long millis = Gosu::milliseconds();

	// Persistent input handling code
	for (it = keystates.begin(); it != keystates.end(); it++) {
		Gosu::Button btn = it->first;
		keystate& state = it->second;
		
		// If there is PERSIST_DELAY_CONSECUTIVE milliseconds of latency
		// between when a button is depressed and when we first look at
		// it here, we'll incorrectly try to fire off a second round of
		// input.
		// This can happen if an intermediary function blocks the thread
		// for a while.
		if (!state.initiallyResolved) {
			state.initiallyResolved = true;
			continue;
		}
		
		int delay = state.consecutive ?
		    ROGUELIKE_PERSIST_DELAY_CONSECUTIVE :
		    ROGUELIKE_PERSIST_DELAY_INIT;
		if (millis >= state.since + delay) {
			state.since = Gosu::milliseconds();
			world->buttonDown(btn);
			if (!state.consecutive)
				state.consecutive = true;
		}
	}
}

