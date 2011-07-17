/******************************
** Tsunagari Tile Engine     **
** world.h                   **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef WORLD_H
#define WORLD_H

#include <string>
#include <vector>

#include "common.h"

namespace Gosu {
	class Button;
}

class Area;
class Player;
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
	World(Resourcer* rc, GameWindow* w);
	
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

	bool loadArea(const std::string& areaName, coord_t playerPos);

private:
	bool processDescriptor();

	Resourcer* rc;
	GameWindow* wnd;
	Area* area;
	boost::scoped_ptr<Player> player;

	//! WorldTypeLocality XML Storage Enum
	/*!
	  Stores the World locality type. Options are "local" (singleplayer),  
	  or "network" (multiplayer).
	*/
	enum WorldTypeLocality {
		LOCAL,
		NETWORK
	};
	
	//! WorldTypeMovement XML Storage Enum
	/*!
	  Stores the World movement type. Options are "turn" (roguelike), 
	  "tile" (yume nikki), or "notile" (zeldalike).
	*/
	enum WorldTypeMovement {
		TURN,
		TILE,
		NOTILE
	};
	
	//! WorldEntry XML Storage Struct
	/*!
	  Stores the World's entry point data. Includes the start
	  Area, and starting coordinates.
	*/
	struct WorldEntry {
		std::string area;
		coord_t coords;
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
		WorldTypeMovement movement;
		WorldEntry entry;
		std::vector<std::string> scripts;
	} xml;
};

#endif

