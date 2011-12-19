/******************************
** Tsunagari Tile Engine     **
** player.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Audio.hpp>
#include <Gosu/Input.hpp>
#include <Gosu/Math.hpp>

#include "area.h"
#include "config.h"
#include "entity.h"
#include "player.h"
#include "world.h"
#include "window.h"

Player::Player(Resourcer* rc, Area* area, ClientValues* conf)
	: Entity(rc, area, conf), velocity(0, 0, 0)
{
}

void Player::startMovement(icoord delta)
{
	switch (conf->moveMode) {
	case TURN:
		moveByTile(delta);
		break;
	case TILE:
		velocity = delta;
		if (velocity)
			moveByTile(velocity);
		break;
	case NOTILE:
		// TODO
		break;
	}
}

void Player::stopMovement(icoord delta)
{
	if (conf->moveMode == TILE) {
		if (velocity == delta)
			velocity = icoord(0, 0, 0);
	}
}

void Player::moveByTile(icoord delta)
{
	if (moving)
		// Support queueing moves?
		return;

	// Left CTRL allows changing facing, but disallows movement.
	const GameWindow& window = GameWindow::getWindow();
	if (window.input().down(Gosu::kbLeftControl)) {
		calculateFacing(delta);
		setPhase(facing);
		redraw = true;
		return;
	}

	icoord newCoord = getTileCoords();
	newCoord += delta;

	// The tile is off the map. Turn to face the direction, but don't move.
	if (!area->tileExists(newCoord)) {
		calculateFacing(delta);
		setPhase(facing);
		redraw = true;
		return;
	}

	destTile = &area->getTile(newCoord);

	// Is anything player-specific preventing us from moving?
	if (destTile->hasFlag(player_nowalk)) {
		// The tile we're trying to move onto is set as player_nowalk.
		// Turn to face the direction, but don't move.
		calculateFacing(delta);
		setPhase(facing);
		redraw = true;
		return;
	}

	Entity::moveByTile(velocity);
}

void Player::preMove(icoord delta)
{
	Entity::preMove(delta);

	SampleRef step = getSound("step");
	if (step)
		step->play(1, 1, 0);

}

void Player::postMove()
{
	Entity::postMove();

	// Doors
	const boost::optional<Door> door = destTile->door;
	if (door) {
		if (!World::getWorld()->loadArea(door->area, door->tile)) {
			// Roll back movement if door failed to open.
			r = fromCoord;
			Log::err("Door", door->area + ": failed to load properly");
		}
	}

	// Layermod
	const boost::optional<int> layermod = destTile->layermod;
	if (layermod) {
		r.z = *layermod;
	}

	// If we have a velocity, keep moving.
	if (conf->moveMode == TILE && velocity)
		moveByTile(velocity);
}

