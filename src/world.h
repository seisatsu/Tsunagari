/*********************************
** Tsunagari Tile Engine        **
** world.h                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef WORLD_H
#define WORLD_H

#include <string>
#include <vector>

#include <boost/scoped_ptr.hpp>
#include <Gosu/Graphics.hpp> // for Gosu::Transform

#include "area.h" // for AreaPtr
#include "common.h"
#include "player.h"
#include "viewport.h"

namespace Gosu {
	class Button;
}

class Area;
class Music;
class Resourcer;
class GameWindow;

//! World Class
/*!
	This class is conceptually the main class of the Tsunagari Tile Engine.
*/
class World
{
public:
	static World* getWorld();

	//! World Constructor
	World(GameWindow* wnd, Resourcer* rc, ClientValues* conf);

	//! World Destructor
	~World();

	//! World Initializer
	bool init();

	//! Gosu Callback
	void buttonDown(const Gosu::Button btn);
	void buttonUp(const Gosu::Button btn);

	//! Gosu Callback
	void draw();

	//! Gosu Callback
	bool needsRedraw() const;

	//! Gosu Callback
	void update(unsigned long dt);

	//! Create a new Area object, loading from the appropriate files, and
	//! set it as the current Area.
	bool loadArea(const std::string& areaName, icoord playerPos);

	//! Given an Area object, set it as the current one.
	void setArea(AreaPtr area, icoord playerPos);

private:
	bool processDescriptor();

	//! Draws black borders around the screen to correct the aspect ratio.
	void drawLetterbox();

	//! Draws a rectangle on the screen of the specified color. Coordinates
	//! are in pixels.
	void drawRect(double x1, double x2, double y1, double y2,
	              Gosu::Color c, double z);

	//! Returns an affine transformation that will zoom and pan the Area to
	//! fit on-screen.
	Gosu::Transform getTransform();

	Resourcer* rc;
	GameWindow* wnd;
	ClientValues* conf;
	Viewport* view;
	AreaPtr area;
	boost::scoped_ptr<Music> music;
	Player player;

	//! WorldTypeLocality XML Storage Enum
	/*!
	  Stores the World locality type. Options are "local" (singleplayer),
	  or "network" (multiplayer).
	*/
	enum WorldTypeLocality {
		LOCAL,
		NETWORK
	};

	//! WorldEntry XML Storage Struct
	/*!
	  Stores the World's entry point data. Includes the start
	  Area, and starting coordinates.
	*/
	struct WorldEntry {
		std::string area;
		icoord coords;
	};

	//! WorldValues XML Storage Struct
	/*!
		Main XML storage struct for World.
	*/
	struct WorldValues {
		std::string name;
		std::string author;
		std::string playerentity;
		WorldTypeLocality locality;
		WorldEntry entry;
		std::string initscript;
		icoord viewport;
	} xml;
};

#endif

