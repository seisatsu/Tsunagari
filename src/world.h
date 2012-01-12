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

#define AREA_ALWAYS_CREATE 0x01

//! World Class
/*!
	This class is the top class handling all data necessary to create a
	game. Such a collection of data is called a "world". Materially, a
	world is just a set of graphics, sound effects, music, and scripts.
	From the perspective from a player, each world should be a separate
	game. Tsunagari is an engine that powers worlds.
*/
class World
{
public:
	//! Get the currently open World.
	static World* getWorld();

	World(GameWindow* wnd, Resourcer* rc, ClientValues* conf);
	~World();

	//! Initialize the world for use.
	bool init();

	//! Process key presses.
	void buttonDown(const Gosu::Button btn);
	void buttonUp(const Gosu::Button btn);

	//! Draw game state to the screen.
	void draw();

	//! Do we need to redraw the screen?
	bool needsRedraw() const;

	//! Update the game world. Process time passing.
	void update(unsigned long dt);

	//! Create a new Area object, loading from the appropriate files. If
	//! the Area has already been loaded previously, return that instance
	//! unless flags contains ALREA_ALWAYS_CREATE.
	AreaPtr getArea(const std::string& filename, int flags = 0);

	//! Switch the game to a new Area, moving the player to the specified
	//! position in the Area.
	void focusArea(AreaPtr area, icoord playerPos);

	//! Get name of script to be run on every Area load.
	std::string getAreaLoadScript();

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

	typedef boost::unordered_map<std::string, AreaPtr> AreaMap;
	AreaMap areas;

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

	std::string name;
	std::string author;
	std::string playerentity;
	WorldTypeLocality locality;
	WorldEntry entry;
	std::string onLoadScript;
	std::string onAreaLoadScript;
	icoord viewport;
};

#endif

