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

	//! Change the graphic. Returns true if it was changed to something
	//  different.
	bool setPhase(const std::string& name);

	//! Retrieve position within Area.
	icoord_t getIPixel() const;
	rcoord_t getRPixel() const;
	icoord_t getTileCoords() const;

	//! Set location within Area.
	void setPixelCoords(icoord_t c);
	void setTileCoords(icoord_t c);

	//! Move within Area.
	void moveByPixel(icoord_t delta);
	void moveByTile(icoord_t delta);

	//! Sets the Area object this entity will ask when looking for
	//  nearby Tiles. Doesn't change x,y,z position.
	void setArea(Area* area);

	//
	// Lua callback targets
	//

	//! Move to the upper left corner. Sets x,y tile positions to 1,1.
	void gotoRandomTile();

	//! Sets speed multiplier.
	void setSpeed(double multiplier);

protected:
	//! Get the Tile we are standing on.
	Tile& getTile();

	SampleRef getSound(const std::string& name);

	//! Calculate which way to face based upon a movement delta.
	void calculateFacing(icoord_t delta);

	//! Called right before starting to moving onto another tile.
	virtual void preMove(icoord_t delta);
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
	bool processSprite(const xmlNode* sprite);
	bool processPhases(const xmlNode* phases);
	bool processPhase(xmlNode* phase, const TiledImage& tiles);
	bool processMember(xmlNode* phase, Animation& anim,
                           const TiledImage& tiles);
	bool processSounds(const xmlNode* sounds);
	bool processSound(xmlNode* sound);
	void processScripts(const xmlNode* scripts);
	void processScript(xmlNode* script);


	Resourcer* rc;

	boost::unordered_map<std::string, Animation> phases;
	Animation* phase;
	bool redraw;
	std::string facing;

	boost::unordered_map<std::string, SampleRef> sounds;
	boost::unordered_map<std::string, std::string> scripts;

	double baseSpeed; //! Original speed, specified in descriptor
	double speedMul;  //! Speed multiplier
	double speed;     //! Effective speed = original speed * multiplier

	bool moving;
	icoord_t dest;
	Tile* movingFrom;

	Area* area;
	icoord_t c;
	rcoord_t r; // real x,y position: hold partial pixel transversal

	std::string descriptor;

	ClientValues* conf;

	//! SpriteValues XML Storage Struct
	/*!
		Main XML storage struct for Sprite.
	*/
	struct SpriteValues {
		std::string sheet;
		icoord_t tileSize;
		boost::unordered_map<std::string, unsigned> phases;
	} xml;
};

#endif

