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

	//! Move the player by dx, dy. Not guaranteed to be smooth.
	void moveByTile(coord_t delta);

protected:
	void preMove(coord_t dest);
	void postMove();

private:
	void normalizeVelocity();

	coord_t velocity;
};

#endif

