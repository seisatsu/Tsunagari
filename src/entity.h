/*********************************
** Tsunagari Tile Engine        **
** entity.h                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

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
	Entity(Area* area);
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

	const std::string getFacing() const;

	//! Change the graphic. Returns true if it was changed to something
	//! different.
	bool setPhase(const std::string& name);

	//! The offset from the upper-left of the Area to the upper-left of the
	//! Tile the Entity is standing on.
	rcoord getPixelCoord() const;

	//! Retrieve position within Area.
	icoord getTileCoords_i() const;
	vicoord getTileCoords_vi() const;

	//! Set location within Area.
	void setTileCoords(icoord phys);
	void setTileCoords(vicoord virt);

	//! Initiate a movement within the Area.
	void moveByTile(ivec2 delta);

	//! Specifies the Area object this entity will ask when looking for
	//! nearby Tiles. Doesn't change x,y,z position.
	void setArea(Area* area);


	//! Move to a random Tile in the Area that is not designated 'nowalk'.
	void gotoRandomTile();

	double getSpeed() const;

	//! Sets speed multiplier.
	void setSpeed(double multiplier);

	//! Get the Tile we are standing on.
	Tile& getTile() const;
	Tile& getTile();

protected:
	std::vector<icoord> frontTiles() const;

	//! Calculate what the draw offset should be and saves it in 'doff'.
	void calcDoff();

	//! Retrieves a sound custom-defined within this Entity's descriptor
	//! file.
	SampleRef getSound(const std::string& name) const;

	//! Normalize each of the X-Y axes into [-1, 0, or 1] and saves value
	//! to 'facing'.
	ivec2 setFacing(ivec2 facing);

	//! Gets a string describing a direction.
	const std::string& directionStr(ivec2 facing) const;

	//! Returns true if we can move in the desired direction.
	virtual bool canMove(icoord delta);

	//! Called right before starting to moving onto another tile.
	virtual void preMove();

	//! Called after we have arrived at another tile.
	virtual void postMove();

	void tileExitScript();
	void tileEntryScript();

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


	//! Set to true if the Entity wants the screen to be redrawn.
	bool redraw;

	typedef boost::unordered_map<std::string, Animation> AnimationMap;
	typedef boost::unordered_map<std::string, SampleRef> SampleMap;
	typedef boost::unordered_map<std::string, std::string> StringMap;

	int imgw, imgh;
	AnimationMap phases;
	Animation* phase;
	ivec2 facing;

	//! List of sounds this Entity knows about.
	SampleMap sounds;
	//! List of scripts this Entity knows about.
	StringMap scripts;

	double baseSpeed; //!< Original speed, specified in descriptor.
	double speedMul;  //!< Speed multiplier.
	double speed;     //!< Effective speed = original speed * multiplier

	//! True if currently moving between two Tiles in an Area. Only used in
	//! TILE game mode.
	bool moving;
	/** Hack flag can be used to not stop the moving animation in-between tiles. */
	bool stillMoving;

	rcoord fromCoord;
	Tile* fromTile;
	rcoord destCoord;
	Tile* destTile;

	//! Pointer to Area this Entity is located on.
	Area* area;
	rcoord r; //!< real x,y position: hold partial pixel transversal
	rcoord doff; //!< Drawing offset to center entity on tile.

	std::string descriptor;
};

void exportEntity();

#endif

