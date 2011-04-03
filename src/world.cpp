#include "world.h"

World::World(GameWindow* window, Resourcer* rc)
{
	player = new Entity(rc, "../testing/outpost_world/player.sheet");
	area = new Area(window, rc, player, "../testing/babysfirst.area");
}

World::~World()
{
	delete player;
	delete area;
}

void World::button_down(Gosu::Button btn)
{
	area->button_down(btn);
}

void World::draw()
{
	area->draw();
}

bool World::needs_redraw()
{
	return area->needs_redraw();
}

