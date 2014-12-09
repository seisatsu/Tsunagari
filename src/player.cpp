/***************************************
** Tsunagari Tile Engine              **
** player.cpp                         **
** Copyright 2011-2013 PariahSoft LLC **
***************************************/

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

#include <Gosu/Audio.hpp>
#include <Gosu/Input.hpp>
#include <Gosu/Math.hpp>

#include "area.h"
#include "client-conf.h"
#include "entity.h"
#include "log.h"
#include "player.h"
#include "world.h"
#include "window.h"

#define ASSERT(x)  if (!(x)) { return false; }

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
	: Character(), velocity(0, 0)
{
	nowalkFlags = TILE_NOWALK | TILE_NOWALK_PLAYER;
	nowalkExempt = TILE_NOWALK_EXIT;
}

bool Player::init(const std::string& descriptor, const std::string& initialPhase)
{
	ASSERT(Entity::init(descriptor));
	setPhase(initialPhase);
	return true;
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
	//
	// FIXME: This may not be the best keybinding because on Mac >= 10.7,
	// Ctrl-Left and Ctrl-Right are bound by default to switch desktop
	// space and do not reach Tsunagari's window.  This leaves the hero in
	// an awkward position of being able to switch directions to up or
	// down but not to left or right.
	// --pdm Dec 6, 2014
	const GameWindow& window = GameWindow::instance();
	if (window.input().down(Gosu::kbLeftControl)) {
		setPhase(directionStr(facing));
		redraw = true;
		return;
	}

	Entity::moveByTile(delta);

	World* world = World::instance();
	world->turn();
}

void Player::useTile()
{
	Tile* t = area->getTile(moveDest(facing));
	if (t)
		t->runUseScript(this);
}

void Player::setFrozen(bool b)
{
	World* world = World::instance();

	b ?  world->storeKeys() : world->restoreKeys();
	Entity::setFrozen(b);
	if (!frozen && velocity)
		moveByTile(velocity);
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
	if (fromTile) {
		ivec2 dxy(deltaCoord.x, deltaCoord.y);
		Exit* exit = fromTile->exitAt(dxy);
		if (exit)
			takeExit(exit);
	}

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
		setTileCoords(fromCoord);
		Log::err("Exit", exit->area + ": failed to load properly");
	}
}

