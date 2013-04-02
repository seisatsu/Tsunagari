/*********************************
** Tsunagari Tile Engine        **
** entity.h                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

// "OmegaSDG" is defined as Michael D. Reiley and Paul Merrill.

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <vector>

#include <boost/unordered_map.hpp>
#include <libxml/parser.h>

#include "tile.h" // for enum TileEventTrigger
#include "reader.h"
#include "scriptinst.h"
#include "python.h"

class Animation;
class Area;

namespace Gosu {
	class Bitmap;
	class Image;
}

enum SetPhaseResult {
	PHASE_NOTFOUND,
	PHASE_NOTCHANGED,
	PHASE_CHANGED
};

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

	virtual void tick(time_t dt);
	void tickTurn(time_t dt);
	void tickTile(time_t dt);
	void tickNoTile(time_t dt);

	void turn();

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

	//! Abstract, Python-specific method.
	virtual void teleport(int x, int y);


	//! Indicates which coordinate we will move into if we proceed in
	//! direction specified.
	icoord moveDest(ivec2 facing);
	vicoord moveDest(Tile* tile, int dx, int dy); // Python-specific version

	//! Returns true if we can move in the desired direction.
	bool canMove(int x, int y, double z); // Python-specific version.
	bool canMove(icoord dest);
	bool canMove(vicoord dest);

	//! Indicates whether we are in the middle of transitioning between
	//! tiles.
	bool isMoving() const;

	//! Initiate a movement within the Area.
	void moveByTile(int x, int y);
	void moveByTile(ivec2 delta);

	//! Abstract, Python-specific method.
	virtual void move(int x, int y);


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
	ScriptInst tickScript, turnScript, tileEntryScript,
	           tileExitScript, deleteScript;


protected:
	virtual void erase();

	//! Precalculate various drawing measurements.
	void calcDraw();

	//! Retrieves a sound custom-defined within this Entity's descriptor
	//! file.
	SampleRef getSound(const std::string& name) const;

	//! Normalize each of the X-Y axes into [-1, 0, or 1] and saves value
	//! to 'facing'.
	ivec2 setFacing(ivec2 facing);

	//! Gets a string describing a direction.
	const std::string& directionStr(ivec2 facing) const;

	enum SetPhaseResult _setPhase(const std::string& name);

	bool nowalked(Tile& t);

	//! Called right before starting to moving onto another tile.
	virtual void preMove();

	//! Called after we have arrived at another tile.
	virtual void postMove();

	void leaveTile();
	void enterTile();
	void enterTile(Tile* t);

	void runTickScript();
	void runTurnScript();
	void runTileExitScript();
	void runTileEntryScript();

	// XML parsing functions used in constructing an Entity
	bool processDescriptor();
	bool processSprite(XMLNode node);
	bool processPhases(XMLNode node, const TiledImageRef& tiles);
	bool processPhase(const XMLNode node, const TiledImageRef& tiles);
	bool processMembers(XMLNode node, std::vector<ImageRef>& frames,
                            const TiledImage& tiles);
	bool processMember(const XMLNode node, std::vector<ImageRef>& frames,
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

	ivec2 imgsz;
	AnimationMap phases;
	Animation* phase;
	std::string phaseName;
	ivec2 facing;

	//! List of sounds this Entity knows about.
	SampleMap sounds;
};

void exportEntity();

#endif

