#include "area.h"
#include "player.h"
#include "sprite.h"
#include "world.h"

Player::Player(Resourcer* rc, Area* area, const std::string& descriptor)
	: Entity(rc, area, descriptor)
{
}

void Player::moveByTile(coord_t delta)
{
	bool changed = false;

	// TODO: use double array of directions
	// would make diagonals easier to handle
	if (delta.x > 0) {
		sprite.setPhase("right");
		changed = true;
	}
	else if (delta.x < 0) {
		sprite.setPhase("left");
		changed = true;
	}
	else if (delta.y > 0) {
		sprite.setPhase("down");
		changed = true;
	}
	else if (delta.y < 0) {
		sprite.setPhase("up");
		changed = true;
	}

	// Redraw the player if we change graphics.
	if (changed)
		redraw = true;

	// Try to actually move.
	coord_t newCoord = sprite.getCoordsByTile();
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

	return Entity::moveByTile(delta);
}

void Player::postMove()
{
	coord_t coord = sprite.getCoordsByTile();
	Area::Tile* dest = area->getTile(coord);
	Area::Door* door = dest->door;
	if (door)
		World::getWorld()->loadArea(door->area, door->coord);
}
