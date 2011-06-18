/******************************
** Tsunagari Tile Engine     **
** window.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <map>
#include <string>
#include <utility>

#include <Gosu/Window.hpp>

namespace Gosu {
	class Button;
}

class Resourcer;
class World;

//! GameWindow Class
/*!
	This class is structurally the main class of the Tsunagari Tile Engine. It handles input and drawing.
*/
class GameWindow : public Gosu::Window
{
public:
	static GameWindow* getWindow();

	//! GameWindow Constructor
	GameWindow(unsigned x, unsigned y, bool fullscreen);

	//! GameWindow Destructor
	virtual ~GameWindow();

	//! GameWindow Initializer
	bool init(const std::string descriptor);

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

private:
	Resourcer* rc;
	World* world;
	
	std::map<Gosu::Button, std::pair<bool, unsigned long> > keystates;
};

#endif

