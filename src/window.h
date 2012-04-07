/*********************************
** Tsunagari Tile Engine        **
** window.h                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <map>
#include <string>
#include <utility>

#include <boost/scoped_ptr.hpp>
#include <Gosu/Window.hpp> // for Gosu::Window

#include "common.h"

namespace Gosu {
	class Button;
}

class Resourcer;
class World;

//! GameWindow Class
/*!
	This class is structurally the main class of the Tsunagari Tile Engine.
	It handles input and drawing.
*/
class GameWindow : public Gosu::Window
{
public:
	static GameWindow& instance();

	//! GameWindow Constructor
	GameWindow();

	//! GameWindow Destructor
	virtual ~GameWindow();

	//! GameWindow Initializer
	bool init(char* argv0);

	//! Width of the window in pixels.
	int width() const;

	//! Height of the window in pixels.
	int height() const;

	//! Gosu Callback
	void buttonDown(const Gosu::Button btn);

	//! Gosu Callback
	void buttonUp(const Gosu::Button btn);

	//! Gosu Callback
	void draw();

	//! Gosu Callback
	bool needsRedraw() const;

	//! Gosu Callback
	void update();

	//! Syncronized time value used throughout the engine
	int time() const;

private:
	//! Calculate time passed since engine state was last updated
	void calculateDt();

	//! Process persistent keyboard input
	void handleKeyboardInput();

	boost::scoped_ptr<Resourcer> rc;
	boost::scoped_ptr<World> world;

	//! Last time engine state was updated.
	int lastTime;

	//! Milliseconds that have passed since engine last updated.
	int dt; // delta time

	//! Milliseconds that have passed since game started. Loops on overflow.
	int now;

	//! Call the garbage colelctor once a second.
	int currentSecond;

	struct keystate {
		bool consecutive, initiallyResolved;
		int since;
	};

	std::map<Gosu::Button, keystate> keystates;
};

#endif

