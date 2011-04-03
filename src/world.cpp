#include "world.h"

World::World(GameWindow* window, Resourcer* rc)
{
	player = new Entity(rc, "../testing/outpost_world/player.sheet");
	area = new Area(window, rc, player, "../testing/babysfirst.area");
}

void World::draw()
{
	area->draw();
}

