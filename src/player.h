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

	//! Smooth continuous movement.
	void startMovement(coord_t delta);
	void stopMovement(coord_t delta);

	//! Move the player by dx, dy. Not guaranteed to be smooth if called
	//  on each update().
	void moveByTile(coord_t delta);

protected:
	void preMove(coord_t delta);
	void postMove();

private:
	void normalizeVelocity();

	//! Stores intent to move continuously in some direction.
	coord_t velocity;
};

#endif

