/******************************
** Tsunagari Tile Engine     **
** tile.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef TILE_H
#define TILE_H

#include <list>

#include "common.h"

class Entity;
class Sprite;

class Tile
{
public:
	Tile(Sprite* s, bool walkable, coord_t coords);
	bool canWalk();
	void draw();
	
	enum TileFlagTypes {
		nowalk,
		player_nowalk,
		npc_nowalk,
		player_event,
		npc_event,
		temp_event
	};
	
	enum TileEventTriggerTypes {
		onUse,
		onEnter,
		onLeave,
		door
	};
	
private:
	Sprite* s;
	bool walkable;
	coord_t coords;

	std::list<Entity*> entities;
};

#endif

