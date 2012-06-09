#include "area.h"
#include "npc.h"

NPC::NPC()
	: Character()
{
}

void NPC::postMove()
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
}

void NPC::takeExit(Exit*)
{
	moving = false; // Prevent time rollover check in Entity::updateTile().
	destroy();
}

