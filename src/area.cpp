#include "area.h"

Area::Area(GameWindow* , Resourcer* , Entity* player, const char* )
{
	this->player = player;
}

void Area::draw()
{
	player->draw();
}

