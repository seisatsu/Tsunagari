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
#include "resourcer.h"
#include "scriptinst.h"
#include "python.h"

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
	Entity();
	virtual ~Entity();

	//! Entity initializer
	virtual bool init(const std::string& descriptor);

	//! Entity destroyer.
	virtual void destroy();

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

	std::string getPhase() const;


	//! The offset from the upper-left of the Area to the upper-left of the
	//! Tile the Entity is standing on.
	rcoord getPixelCoord() const;

	//! Retrieve position within Area.
	icoord getTileCoords_i() const;
	vicoord getTileCoords_vi() const;

	//! Set location within Area.
	void setTileCoords(int x, int y);
	void setTileCoords(int x, int y, double z);
	void setTileCoords(icoord phys);
	void setTileCoords(vicoord virt);
	void setTileCoords(rcoord virt);


	//! Indicates which coordinate we will move into if we proceed in
	//! direction specified.
	icoord moveDest(ivec2 facing);
	vicoord moveDest(Tile* tile, int dx, int dy); // Python-specific version

	//! Returns true if we can move in the desired direction.
	bool canMove(int x, int y, double z); // Python-specific version.
	bool canMove(icoord delta);

	//! Indicates whether we are in the middle of transitioning between
	//! tiles.
	bool isMoving() const;

	//! Initiate a movement within the Area.
	void moveByTile(int x, int y);
	void moveByTile(ivec2 delta);


	//! Gets the Entity's current Area.
	Area* getArea();

	//! Specifies the Area object this entity will ask when looking for
	//! nearby Tiles. Doesn't change x,y,z position.
	void setArea(Area* area);


	//! Gets speed multiplier.
	double getSpeed() const;

	//! Sets speed multiplier.
	void setSpeed(double multiplier);


	//! Get the Tile that we are standing on.
	Tile* getTile() const;
	Tile* getTile();

	virtual void setFrozen(bool b);
	bool getFrozen();


	//
	// Python-specific interface
	//

	//! Exempt ourselves from TILE_NOWALK et al.
	FlagManip exemptManip();


	//
	// Variables public for Python scripts
	//

	//! Script hooks.
	ScriptInst updateScript, tileEntryScript, tileExitScript, deleteScript;


protected:
	std::vector<icoord> frontTiles(Tile* tile, ivec2 facing) const;

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

	bool nowalked(Tile& t);

	//! Called right before starting to moving onto another tile.
	virtual void preMove();

	//! Called after we have arrived at another tile.
	virtual void postMove();

	void leaveTile();
	void enterTile();
	void enterTile(Tile* t);

	void runUpdateScript();
	void runTileExitScript();
	void runTileEntryScript();

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
	bool setScript(const std::string& trigger, ScriptInst& script);


protected:
	typedef boost::unordered_map<std::string, Animation> AnimationMap;
	typedef boost::unordered_map<std::string, SampleRef> SampleMap;


	//! Set to true if the Entity wants the screen to be redrawn.
	bool redraw;

	//! Pointer to Area this Entity is located on.
	Area* area;
	rcoord r; //!< real x,y position: hold partial pixel transversal
	rcoord doff; //!< Drawing offset to center entity on tile.

	std::string descriptor;

	bool frozen;

	double baseSpeed; //!< Original speed, specified in descriptor.
	double speedMul;  //!< Speed multiplier.
	double speed;     //!< Effective speed = original speed * multiplier

	//! True if currently moving between two Tiles in an Area. Only used in
	//! TILE game mode.
	bool moving;
	/** Hack flag can be used to not stop the moving animation in-between
	 * tiles. */
	bool stillMoving;
	unsigned nowalkFlags;
	unsigned nowalkExempt;

	vicoord deltaCoord;
	rcoord fromCoord;
	rcoord destCoord;
	Tile* fromTile;
	Tile* destTile;

	int imgw, imgh;
	AnimationMap phases;
	Animation* phase;
	std::string phaseName;
	ivec2 facing;

	//! List of sounds this Entity knows about.
	SampleMap sounds;
};

void exportEntity();

#endif

