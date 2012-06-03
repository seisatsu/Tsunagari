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


Player::Player()
	: Entity(), velocity(0, 0)
{
	nowalkFlags = TILE_NOWALK | TILE_NOWALK_PLAYER;
	nowalkExempt = TILE_NOWALK_EXIT | TILE_NOWALK_AREA_BOUND;
}

bool Player::init(const std::string& descriptor)
{
	bool b = Entity::init(descriptor);
	if (b) {
		// Set an initial phase.
		setPhase(directionStr(setFacing(ivec2(0, 1))));
	}
	return b;
}

void Player::destroy()
{
	// Assuming this is being called from Python code, where this function
	// is bound to the name "delete()".
	Log::err("Player", "delete(): Player cannot be destroyed");
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
	stillMoving = velocity;
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
	stillMoving = velocity;
}

void Player::moveByTile(ivec2 delta)
{
	if (frozen)
		return;
	if (moving)
		return;

	setFacing(delta);

	// Left CTRL allows changing facing, but disallows movement.
	const GameWindow& window = GameWindow::instance();
	if (window.input().down(Gosu::kbLeftControl)) {
		setPhase(directionStr(facing));
		redraw = true;
		return;
	}

	Entity::moveByTile(delta);
}

void Player::useTile()
{
	std::vector<icoord> tiles = frontTiles();
	BOOST_FOREACH(icoord& c, tiles) {
		Tile* t = area->getTile(c);
		t->runUseScript(this);
	}
}

void Player::setFrozen(bool b)
{
	movements.clear();

	Entity::setFrozen(b);
}

void Player::postMove()
{
	Entity::postMove();

	// Normal exit.
	if (destTile) {
		Exit* exit = destTile->exits[EXIT_NORMAL];
		if (exit)
			takeExit(exit);
	}

	// Side exit.
	ivec2 dxy(deltaCoord.x, deltaCoord.y);
	Exit* exit = fromTile->exitAt(dxy);
	if (exit)
		takeExit(exit);

	// If we have a velocity, keep moving.
	if (conf.moveMode == TILE && velocity)
		moveByTile(velocity);
}

void Player::takeExit(Exit* exit)
{
	World* world = World::instance();
	Area* newArea = world->getArea(exit->area);
	if (newArea) {
		world->focusArea(newArea, exit->coords);
	}
	else {
		// Roll back movement if exit failed to open.
		r = fromCoord;
		Log::err("Exit", exit->area + ": failed to load properly");
	}
}
