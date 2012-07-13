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

	//! Time since epoch.
	time_t time() const;

private:
	//! Process persistent keyboard input
	void handleKeyboardInput(time_t now);

	boost::scoped_ptr<Resourcer> rc;
	boost::scoped_ptr<World> world;

	time_t now;
	time_t lastGCtime;

	struct keystate {
		bool consecutive, initiallyResolved;
		time_t since;
	};

	std::map<Gosu::Button, keystate> keystates;
};

#endif

