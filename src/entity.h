/******************************
** Tsunagari Tile Engine     **
** entity.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>

#include <boost/unordered_map.hpp>
#include <libxml/parser.h>

#include "tile.h" // for enum TileEventTrigger
#include "common.h"
#include "resourcer.h"

class Animation;
class Area;
class Resourcer;

namespace Gosu {
	class Bitmap;
	class Image;
}

//! An Entity represents one 'thing' that will be rendered to the screen.
/*!
	An Entity might be a dynamic game object such as a monster, NPC, or
	item.  Entity can handle animated images that cycle through their
	frames over time. It also has the capacity to switch between a couple
	different images on demand.

	For example, you might have a Entity for a player character with
	animated models for walking in each possible movement direction (up,
	down, left, right) along with static standing-still images for each
	direction.
*/
class Entity
{
public:
	Entity(Resourcer* rc, Area* area, ClientValues* conf);
	~Entity();

	//! Entity Initializer
	bool init(const std::string& descriptor);

	//! Gosu Callback
	void draw();
	bool needsRedraw() const;

	void update(unsigned long dt);
	void updateTurn(unsigned long dt);
	void updateTile(unsigned long dt);
	void updateNoTile(unsigned long dt);

	//! Change the graphic. Returns true if it was changed to something
	//! different.
	bool setPhase(const std::string& name);

	//! The offset from the upper-left of the Area to the upper-left of the
	//! Tile the Entity is standing on.
	rcoord getPixelCoord() const;

	//! Retrieve position within Area.
	icoord getTileCoords() const;

	//! Set location within Area.
	void setPixelCoords(icoord c);
	void setTileCoords(icoord c);

	//! Move within Area.
	void moveByTile(icoord delta);

	//! Sets the Area object this entity will ask when looking for
	//! nearby Tiles. Doesn't change x,y,z position.
	void setArea(Area* area);

	//
	// Lua callback targets
	//

	//! Move to the upper left corner. Sets x,y tile positions to 1,1.
	void gotoRandomTile();

	//! Sets speed multiplier.
	void setSpeed(double multiplier);

protected:
	//! Calculate what doff should be and save it in doff.
	void calcDoff();

	//! Get the Tile we are standing on.
	Tile& getTile();

	SampleRef getSound(const std::string& name);

	//! Calculate which way to face based upon a movement delta.
	void calculateFacing(icoord delta);

	//! Called right before starting to moving onto another tile.
	virtual void preMove(icoord delta);
	virtual void preMoveLua();

	void leaveTile();
	void enterTile();
	void postMoveScript();

	//! Called after we have arrived at another tile.
	virtual void postMove();
	virtual void postMoveLua();

	void tileScripts(Tile& tile, std::vector<TileEvent>& events,
	                 TileEventTrigger trigger);
	void runTileLua(Tile& tile, const std::string& script);

	// XML parsing functions used in constructing an Entity
	bool processDescriptor();
	bool processSprite(XMLNode node);
	bool processPhases(XMLNode node, const TiledImage& tiles);
	bool processPhase(const XMLNode node, const TiledImage& tiles);
	bool processMembers(XMLNode node, Animation& anim,
                            const TiledImage& tiles);
	bool processMember(const XMLNode node, Animation& anim,
                           const TiledImage& tiles);
	bool processSounds(XMLNode node);
	bool processSound(const XMLNode node);
	bool processScripts(XMLNode node);
	bool processScript(const XMLNode node);


	//! Pointer to local resource object. Loads images, sounds, and scripts
	//! with it.
	Resourcer* rc;

	//! Set to true if the Entity wants the screen to be redrawn.
	bool redraw;

	int imgw, imgh;
	boost::unordered_map<std::string, Animation> phases;
	Animation* phase;
	std::string facing;

	//! List of sounds this Entity knows about.
	boost::unordered_map<std::string, SampleRef> sounds;

	//! List of scripts this Entity knows about.
	boost::unordered_map<std::string, std::string> scripts;

	double baseSpeed; //!< Original speed, specified in descriptor.
	double speedMul;  //!< Speed multiplier.
	double speed;     //!< Effective speed = original speed * multiplier

	//! True if currently moving between two Tiles in an Area. Only used in
	//! TILE game mode.
	bool moving;

	icoord fromCoord;
	Tile* fromTile;
	icoord destCoord;
	Tile* destTile;

	//! Pointer to Area this Entity is located on.
	Area* area;
	icoord c;
	rcoord r; //!< real x,y position: hold partial pixel transversal
	icoord doff; //!< Drawing offset to center entity on tile.

	std::string descriptor;

	//! Pointer to game configuration.
	ClientValues* conf;
};

#endif

