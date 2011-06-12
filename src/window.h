/******************************
** Tsunagari Tile Engine     **
** window.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <string>

#include <Gosu/Gosu.hpp>

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
	GameWindow(uint x, uint y, bool fullscreen);

	//! GameWindow Destructor
	virtual ~GameWindow();

	//! GameWindow Initializer
	bool init(const std::string descriptor);

	//! Gosu Callback
	void buttonDown(const Gosu::Button btn);

	//! Gosu Callback
	void draw();

	//! Gosu Callback
	bool needsRedraw() const;

	//! Gosu Callback
	void update();

private:
	Resourcer* rc;
	World* world;
};

#endif

