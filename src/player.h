/******************************
** Tsunagari Tile Engine     **
** player.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

class Player : public Entity
{
public:
	Player(Resourcer* rc, Area* area);

	//! Move the entity by dx, dy.
	void moveByTile(coord_t delta);

private:
	void postMove();
};

#endif

