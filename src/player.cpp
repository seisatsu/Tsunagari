/*********************************
** Tsunagari Tile Engine        **
** player.cpp                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <Gosu/Audio.hpp>
#include <Gosu/Input.hpp>
#include <Gosu/Math.hpp>

#include "area.h"
#include "config.h"
#include "entity.h"
#include "log.h"
#include "player.h"
#include "world.h"
#include "window.h"

template<class Cont, class ValueType>
void removeValue(Cont* c, ValueType v)
{
	typename Cont::iterator it;
	for (it = c->begin(); it != c->end(); ++it) {
		if (*it == v) {
			c->erase(it);
			return;
		}
	}
}


Player::Player(Area* area)
	: Entity(area), velocity(0, 0)
{
}

void Player::startMovement(ivec2 delta)
{
	switch (conf.moveMode) {
	case TURN:
		moveByTile(delta);
		break;
	case TILE:
		movements.push_back(delta);
		velocity = delta;
		moveByTile(velocity);
		break;
	case NOTILE:
		// TODO
		break;
	}
}

void Player::stopMovement(ivec2 delta)
{
	switch (conf.moveMode) {
	case TURN:
		break;
	case TILE:
		removeValue(&movements, delta);
		velocity = movements.size() ?
		           movements.back() :
			   ivec2(0, 0);
		if (velocity)
			moveByTile(velocity);
		break;
	case NOTILE:
		// TODO
		break;
	}
}

void Player::moveByTile(ivec2 delta)
{
	if (moving)
		// Support queueing moves?
		return;

	// Left CTRL allows changing facing, but disallows movement.
	const GameWindow& window = GameWindow::getWindow();
	if (window.input().down(Gosu::kbLeftControl)) {
		setPhase(directionStr(setFacing(delta)));
		redraw = true;
		return;
	}

	// FIXME: use frontTiles()
	icoord newCoord = getTileCoords_i();
	newCoord += icoord(delta.x, delta.y, 0);

	// The tile is off the map. Turn to face the direction, but don't move.
	if (!area->inBounds(newCoord)) {
		setPhase(directionStr(setFacing(delta)));
		redraw = true;
		return;
	}

	destTile = &area->getTile(newCoord);

	// Is anything player-specific preventing us from moving?
	if (destTile->hasFlag(player_nowalk)) {
		// The tile we're trying to move onto is set as player_nowalk.
		// Turn to face the direction, but don't move.
		setPhase(directionStr(setFacing(delta)));
		redraw = true;
		return;
	}

	Entity::moveByTile(delta);
}

void Player::useTile()
{
	std::vector<icoord> tiles = frontTiles();
	BOOST_FOREACH(icoord& c, tiles) {
		Tile& t = area->getTile(c);
		t.onUseScripts(this);
	}
}

void Player::preMove()
{
	Entity::preMove();

	SampleRef step = getSound("step");
	if (step)
		step->play();

}

void Player::postMove()
{
	Entity::postMove();

	// Doors
	const boost::optional<Door> door = destTile->door;
	if (door) {
		World* world = World::getWorld();
		AreaPtr newArea = world->getArea(door->area);
		if (newArea) {
			world->focusArea(newArea, door->tile);
		}
		else {
			// Roll back movement if door failed to open.
			r = fromCoord;
			Log::err("Door",
			         door->area + ": failed to load properly");
		}
	}

	vicoord tile = getTileCoords_vi();
	Log::dev("Player", boost::str(
		boost::format("location x:%d y:%d z:%.1f")
		  % tile.x % tile.y % r.z)
	);

	// If we have a velocity, keep moving.
	if (conf.moveMode == TILE && velocity)
		moveByTile(velocity);
}

