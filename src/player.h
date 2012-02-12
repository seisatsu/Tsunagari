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
	Player(Area* area);

	//! Smooth continuous movement.
	void startMovement(ivec2 delta);
	void stopMovement(ivec2 delta);

	//! Move the player by dx, dy. Not guaranteed to be smooth if called
	//! on each update().
	void moveByTile(ivec2 delta);

	//! Try to use an object in front of the player.
	void useTile();

protected:
	void preMove();
	void postMove();

private:
	//! Stores intent to move continuously in some direction.
	ivec2 velocity;

	//! Stack storing depressed keyboard keys in the form of movement
	//! vectors.
	std::vector<ivec2> movements;
};

#endif

