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
	: Entity(rc, area, conf), velocity(icoord(0, 0, 0))
{
}

void Player::startMovement(icoord delta)
{
	if (conf->moveMode == TURN) {
		// TODO Move by velocity would allow true diagonal movement
		moveByTile(delta);
	}
	else if (conf->moveMode == TILE) {
		velocity.x += delta.x;
		velocity.y += delta.y;
		velocity.z += delta.z;
		normalizeVelocity();
		moveByTile(velocity);
	}
}

void Player::stopMovement(icoord delta)
{
	if (conf->moveMode == TILE) {
		velocity.x -= delta.x;
		velocity.y -= delta.y;
		velocity.z -= delta.z;
		normalizeVelocity();
		if (velocity.x || velocity.y || velocity.z)
			moveByTile(velocity);
	}
}

void Player::moveByTile(icoord delta)
{
	// You can't interrupt an in-progress movement.
	if (moving)
		return;

	// Left CTRL allows changing facing, but disallows movement.
	const GameWindow& window = GameWindow::getWindow();
	if (window.input().down(Gosu::kbLeftControl)) {
		calculateFacing(delta);
		setPhase(facing);
		return;
	}

	// Try to actually move.
	icoord newCoord = getTileCoords();
	newCoord.x += delta.x;
	newCoord.y += delta.y;
	newCoord.z += delta.z;
	const Tile& dest = area->getTile(newCoord);
	if ((dest.flags       & player_nowalk) != 0 ||
	    (dest.type->flags & player_nowalk) != 0) {
		// The tile we're trying to move onto is set as player_nowalk.
		// Turn to face the direction, but don't move.
		calculateFacing(delta);
		setPhase(facing);
		return;
	}

	Entity::moveByTile(delta);
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

	const icoord coord = getTileCoords();
	const Tile& dest = area->getTile(coord);
	const boost::optional<Door> door = dest.door;
	if (door) {
		if (!World::getWorld()->loadArea(door->area, door->tile)) {
			// Roll back movement if door failed to open.
			c = fromCoord;
			r.x = c.x;
			r.y = c.y;
			r.z = c.z;
			Log::err("Area", door->area + ": failed to load properly");
		}
	}
	if (conf->moveMode == TILE)
		if (velocity.x || velocity.y || velocity.z)
			moveByTile(velocity);
}

void Player::normalizeVelocity()
{
	velocity.x = Gosu::boundBy(velocity.x, -1, 1);
	velocity.y = Gosu::boundBy(velocity.y, -1, 1);
	velocity.z = Gosu::boundBy(velocity.z, -1, 1);
}

