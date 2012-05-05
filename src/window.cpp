/*********************************
** Tsunagari Tile Engine        **
** window.cpp                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <boost/foreach.hpp>
#include <Gosu/Graphics.hpp> // for Gosu::Graphics
#include <Gosu/Timing.hpp>
#include <Gosu/Utility.hpp>

#include "config.h"
#include "resourcer.h"
#include "world.h"
#include "window.h"

namespace Gosu {
	/**
	 * Enable 1980s-style graphics scaling: nearest-neighbor filtering.
	 * Call this function before creating any Gosu::Image.
	 */
	void enableUndocumentedRetrofication() {
		extern bool undocumentedRetrofication;
		undocumentedRetrofication = true;
	}
}

static GameWindow* globalWindow = NULL;

GameWindow& GameWindow::instance()
{
	return *globalWindow;
}

GameWindow::GameWindow()
	// Gosu emulates the requested screen resolution on fullscreen,
	// but this breaks our aspect ratio-correcting letterbox.
	// Ergo we just make a window the size of the screen.
	: Gosu::Window(
	    conf.fullscreen ? Gosu::screenWidth() :
	                      (unsigned)conf.windowSize.x,
	    conf.fullscreen ? Gosu::screenHeight() :
	                      (unsigned)conf.windowSize.y,
	    conf.fullscreen
	  ),
	  lastTime((int)Gosu::milliseconds()),
	  now(lastTime),
	  currentSecond(now/1000)
{
	globalWindow = this;
	Gosu::enableUndocumentedRetrofication();
}

GameWindow::~GameWindow()
{
}

bool GameWindow::init(char* argv0)
{
	rc.reset(new Resourcer());
	world.reset(new World());
	if (!rc->init(argv0))
	       return false;
	       
	bool cacheEnabled = conf.cacheEnabled;
	conf.cacheEnabled = false;

	BOOST_FOREACH(std::string pathname, conf.dataPath) {
		if (!rc->prependPath(pathname))
			return false;
		if (rc->resourceExists("init.py"))
			rc->runPythonScript("init.py");
		else {
			Log::fatal(pathname, "couldn't find init.py");
			return false;
		}
	}

	if (!rc->appendPath(BASE_ZIP))
		return false;
	if (rc->resourceExists("init.py"))
		rc->runPythonScript("init.py");
	else {
		Log::fatal(BASE_ZIP, "couldn't find init.py");
		return false;
	}
	
	conf.cacheEnabled = cacheEnabled;

	if (!rc->appendPath(conf.worldFilename))
		return false;
	return world->init();
}

int GameWindow::width() const
{
	return (int)graphics().width();
}

int GameWindow::height() const
{
	return (int)graphics().height();
}

void GameWindow::buttonDown(const Gosu::Button btn)
{
	now = (int)Gosu::milliseconds();
	if (btn == Gosu::kbEscape)
		close();
	else {
		if (keystates.find(btn) == keystates.end()) {
			keystate& state = keystates[btn];
			state.since = now;
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
	world->buttonUp(btn);
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
	if (conf.moveMode == TURN)
		handleKeyboardInput();
	world->update(dt);

	// Run once per second.
	if (now/1000 > currentSecond) {
		currentSecond = now/1000;
		rc->garbageCollect();
	}
}

int GameWindow::time() const
{
	return now;
}

void GameWindow::calculateDt()
{
	now = (int)Gosu::milliseconds();
	dt = now - lastTime;
	lastTime = now;
}

void GameWindow::handleKeyboardInput()
{
	std::map<Gosu::Button, keystate>::iterator it;

	// Persistent input handling code
	for (it = keystates.begin(); it != keystates.end(); it++) {
		Gosu::Button btn = it->first;
		keystate& state = it->second;

		// If there is persistCons milliseconds of latency
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
		    conf.persistCons : conf.persistInit;
		if (now >= state.since + delay) {
			state.since = now;
			world->buttonDown(btn);
			state.consecutive = true;
		}
	}
}

