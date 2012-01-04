/*********************************
** Tsunagari Tile Engine        **
** player.h                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef PLAYER_H
#define PLAYER_H

#include <vector>

#include "common.h"
#include "entity.h"

class Area;
class Resourcer;

class Player : public Entity
{
public:
	Player(Resourcer* rc, Area* area, ClientValues* conf);

	//! Smooth continuous movement.
	void startMovement(icoord delta);
	void stopMovement(icoord delta);

	//! Move the player by dx, dy. Not guaranteed to be smooth if called
	//! on each update().
	void moveByTile(icoord delta);

	//! Try to use an object in front of the player.
	void useTile();

protected:
	void preMove();
	void postMove();

private:
	//! Stores intent to move continuously in some direction.
	icoord velocity;

	//! Stack storing depressed keyboard keys in the form of movement
	//! vectors.
	std::vector<icoord> movements;
};

#endif

