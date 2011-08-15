/******************************
** Tsunagari Tile Engine     **
** player.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"
#include "entity.h"

class Area;
class Resourcer;

class Player : public Entity
{
public:
	Player(Resourcer* rc, Area* area, ClientValues* conf);

	//! Smooth continuous movement.
	void startMovement(icoord_t delta);
	void stopMovement(icoord_t delta);

	//! Move the player by dx, dy. Not guaranteed to be smooth if called
	//  on each update().
	void moveByTile(icoord_t delta);

protected:
	void preMove(icoord_t delta);
	void postMove();

private:
	void normalizeVelocity();

	//! Stores intent to move continuously in some direction.
	icoord_t velocity;
};

#endif

