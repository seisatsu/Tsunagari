#include "area.h"

Area::Area(GameWindow* , Resourcer* , Entity* player, const char* )
{
	this->player = player;
}

Area::~Area()
{
}

void Area::draw()
{
	player->draw();
}

bool Area::needs_redraw()
{
	return player->needs_redraw();
}

