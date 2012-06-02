/*********************************
** Tsunagari Tile Engine        **
** world.h                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef WORLD_H
#define WORLD_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <Gosu/Graphics.hpp> // for Gosu::Transform

#include "player.h"
#include "scriptinst.h"
#include "viewport.h"

namespace Gosu {
	class Button;
}

class Area;
class Music;
class Resourcer;
class GameWindow;

/**
 * Top class holding all data necessary to create a game. Such a collection of
 * data is called a "world". Materially, a world is just a set of graphics,
 * sound effects, music, and scripts. From the perspective from a player, each
 * world should be a separate game. Tsunagari is an engine that powers worlds.
 */
class World
{
public:
	//! Get the currently open World.
	static World* instance();

	World();
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
	//! the Area has already been loaded previously, return that instance.
	Area* getArea(const std::string& filename);

	//! Returns the currently focused Area.
	Area* getFocusedArea();

	//! Switch the game to a new Area, moving the player to the specified
	//! position in the Area.
	void focusArea(Area* area, int x, int y, double z);
	void focusArea(Area* area, vicoord playerPos);

	//! Get name of script to be run on every Area load.
	ScriptInst& getAreaLoadScript();

private:
	//! Draws black borders around the screen to correct the aspect ratio.
	//! Draws black borders around the Area if the Area doesn't fit in the
	//! Viewport.
	void pushLetterbox();
	void popLetterbox();

	//! Draws a rectangle on the screen of the specified color. Coordinates
	//! are in pixels.
	void drawRect(double x1, double x2, double y1, double y2,
	              Gosu::Color c, double z);

	//! Returns an affine transformation that will zoom and pan the Area to
	//! fit on-screen.
	Gosu::Transform getTransform();

	bool processDescriptor();
	bool processInfo(XMLNode node);
	bool processInit(XMLNode node);
	bool processScript(XMLNode node);
	bool processInput(XMLNode node);

	boost::shared_ptr<Viewport> view;
	Area* area;
	boost::shared_ptr<Music> music;
	Player player;
	Resourcer* rc;
	int clips;

	typedef boost::unordered_map<std::string, Area*> AreaMap;
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
		vicoord coords;
	};

	std::string name;
	std::string author;
	double version;
	std::string playerentity;
	WorldTypeLocality locality;
	WorldEntry entry;
	ScriptInst onLoadScript;
	ScriptInst onAreaLoadScript;
	icoord viewport;
};

void exportWorld();

#endif

