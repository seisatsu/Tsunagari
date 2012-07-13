/*********************************
** Tsunagari Tile Engine        **
** world.h                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef WORLD_H
#define WORLD_H

#include <stack>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <Gosu/Graphics.hpp> // for Gosu::Transform

#include "bitrecord.h"
#include "player.h"
#include "resourcer.h"
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
	/**
	 * Get the currently open World.
	 */
	static World* instance();

	World();
	~World();

	/**
	 * Initialize the world for use.
	 */
	bool init();

	/**
	 * User's friendly name for this map.
	 */
	const std::string& getName() const;

	/**
	 * Syncronized time value used throughout the engine.
	 */
	time_t time() const;

	/**
	 * Process key presses.
	 */
	void buttonDown(const Gosu::Button btn);
	void buttonUp(const Gosu::Button btn);

	/**
	 * Draw game state to the screen.
	 */
	void draw();

	/**
	 * Do we need to redraw the screen?
	 */
	bool needsRedraw() const;

	void update(time_t now);

	/**
	 * Updates the game state within this World as if dt milliseconds had
	 * passed since the last call.
	 *
	 *                       MOVE MODE
	 *                 TURN     TILE     NOTILE
	 * Area		   yes      yes      yes
	 * Character       no       yes      yes
	 * Overlay         yes      yes      yes
	 */
	void tick(unsigned long dt);

	/**
	 * Update the game world when the turn is over (Player moves).
	 *
	 *                       MOVE MODE
	 *                 TURN     TILE     NOTILE
	 * Area		   yes      no       no
	 * Character       yes      no       no
	 * Overlay         yes      no       no
	 */
	void turn();

	/**
	 * Create a new Area object, loading from the appropriate files. If
	 * the Area has already been loaded previously, return that instance.
	 */
	Area* getArea(const std::string& filename);

	/**
	 * Returns the currently focused Area.
	 */
	Area* getFocusedArea();

	/**
	 * Switch the game to a new Area, moving the player to the specified
	 * position in the Area.
	 */
	void focusArea(Area* area, int x, int y, double z);
	void focusArea(Area* area, vicoord playerPos);

	void setPaused(bool b);

	void storeKeys();
	void restoreKeys();

	void runAreaLoadScript(Area* area);

protected:
	/**
	 * Calculate time passed since engine state was last updated.
	 */
	time_t calculateDt(time_t now);

	/**
	 * Draws black borders around the screen. Used to correct the aspect
	 * ratio and optimize drawing if the Area doesn't fit into the
	 * Viewport.
	 */
	int pushLetterbox();
	void popLetterbox(int clips);

	/**
	 * Draws a rectangle on the screen of the specified color. Coordinates
	 * are in pixels.
	 */
	void drawRect(double x1, double x2, double y1, double y2,
	              Gosu::Color c, double z);

	/**
	 * Returns an affine transformation that will zoom and pan the Area to
	 * fit on-screen.
	 */
	Gosu::Transform getTransform();

	bool processDescriptor();
	bool processInfo(XMLNode node);
	bool processInit(XMLNode node);
	bool processScript(XMLNode node);
	bool processInput(XMLNode node);

protected:
	typedef boost::unordered_map<std::string, Area*> AreaMap;


	std::string name;
	std::string author;
	double version;
	icoord viewportSz;


	std::string playerEntity;
	std::string startArea;
	vicoord startCoords;


	ScriptInst loadScript;
	ScriptInst areaLoadScript;
	ImageRef pauseInfo;


	AreaMap areas;
	Area* area;
	Player player;
	boost::shared_ptr<Viewport> view;
	boost::shared_ptr<Music> music;
	Resourcer* rc;


	/**
	 * Last time engine state was updated. See World::update().
	 */
	time_t lastTime;

	/**
	 * Total unpaused game run time.
	 */
	time_t total;


	bool redraw;
	bool userPaused;
	int paused;


	std::stack<BitRecord> keyStates;
};

void exportWorld();

#endif

