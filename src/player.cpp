/******************************
** Tsunagari Tile Engine     **
** player.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Audio.hpp>
#include <Gosu/Input.hpp>
#include <Gosu/Math.hpp>

#include "area.h"
#include "entity.h"
#include "player.h"
#include "world.h"
#include "window.h"

Player::Player(Resourcer* rc, Area* area)
	: Entity(rc, area), velocity(coord(0, 0, 0))
{
}

void Player::startMovement(coord_t delta)
{
	velocity.x += delta.x;
	velocity.y += delta.y;
	velocity.z += delta.z;
	normalizeVelocity();
	moveByTile(velocity);
}

void Player::stopMovement(coord_t delta)
{
	velocity.x -= delta.x;
	velocity.y -= delta.y;
	velocity.z -= delta.z;
	normalizeVelocity();
}

void Player::moveByTile(coord_t delta)
{
	// You can't stop an in-progress movement.
	if (moving)
		return;

	// TODO: use double array of directions
	// would make diagonals easier to handle
	if (delta.x > 0) {
		setPhase("right");
		redraw = true;
	}
	else if (delta.x < 0) {
		setPhase("left");
		redraw = true;
	}
	else if (delta.y > 0) {
		setPhase("down");
		redraw = true;
	}
	else if (delta.y < 0) {
		setPhase("up");
		redraw = true;
	}

	// Left CTRL allows changing facing, but disallows movement.
	const GameWindow& window = GameWindow::getWindow();
	if (window.input().down(Gosu::kbLeftControl))
		return;

	// Try to actually move.
	coord_t newCoord = getCoordsByTile();
	newCoord.x += delta.x;
	newCoord.y += delta.y;
	newCoord.z += delta.z;
	const Area::Tile& dest = area->getTile(newCoord);
	if ((dest.flags       & Area::player_nowalk) != 0 ||
	    (dest.type->flags & Area::player_nowalk) != 0) {
		// The tile we're trying to move onto is set as player_nowalk.
		// Stop here.
		return;
	}

	Entity::moveByTile(delta);
}

void Player::preMove(coord_t)
{
	SampleRef step = getSound("step");
	if (step)
		step->play(1, 1, 0);
}

void Player::postMove()
{
	const coord_t coord = getCoordsByTile();
	const Area::Tile& dest = area->getTile(coord);
	const boost::optional<Area::Door> door = dest.door;
	if (door)
		World::getWorld()->loadArea(door->area, door->coord);
	if (velocity.x || velocity.y || velocity.z)
		moveByTile(velocity);
}

void Player::normalizeVelocity()
{
	velocity.x = Gosu::boundBy(velocity.x, -1L, 1L);
	velocity.y = Gosu::boundBy(velocity.y, -1L, 1L);
	velocity.z = Gosu::boundBy(velocity.z, -1L, 1L);
}

