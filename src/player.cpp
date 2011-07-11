/******************************
** Tsunagari Tile Engine     **
** player.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Input.hpp>

#include "area.h"
#include "entity.h"
#include "player.h"
#include "world.h"
#include "window.h"

Player::Player(Resourcer* rc, Area* area)
	: Entity(rc, area)
{
}

void Player::moveByTile(coord_t delta)
{
	bool changed = false;

	// TODO: use double array of directions
	// would make diagonals easier to handle
	if (delta.x > 0) {
		setPhase("right");
		changed = true;
	}
	else if (delta.x < 0) {
		setPhase("left");
		changed = true;
	}
	else if (delta.y > 0) {
		setPhase("down");
		changed = true;
	}
	else if (delta.y < 0) {
		setPhase("up");
		changed = true;
	}

	// Redraw the player if we change graphics.
	if (changed)
		redraw = true;

	GameWindow* w = GameWindow::getWindow();
	if (w->input().down(Gosu::kbLeftControl))
		return;

	// Try to actually move.
	coord_t newCoord = getCoordsByTile();
	newCoord.x += delta.x;
	newCoord.y += delta.y;
	newCoord.z += delta.z;
	Area::Tile* dest = area->getTile(newCoord);
	if ((dest->flags       & Area::player_nowalk) != 0 ||
	    (dest->type->flags & Area::player_nowalk) != 0) {
		// The tile we're trying to move onto is set as player_nowalk.
		// Stop here.
		return;
	}

	Entity::moveByTile(delta);
}

void Player::postMove()
{
	coord_t coord = getCoordsByTile();
	Area::Tile* dest = area->getTile(coord);
	Area::Door* door = dest->door;
	if (door)
		World::getWorld()->loadArea(door->area, door->coord);
}

