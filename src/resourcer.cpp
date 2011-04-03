/******************************
** Tsunagari Tile Engine     **
** resourcer.cpp             **
** Copyright 2011 OmegaSDG   **
******************************/

#include "resourcer.h"

Resourcer::Resourcer(GameWindow* window/*, string filename*/)
{
	this->window = window;
}

Gosu::Image* Resourcer::get_image(wstring name)
{
	return new Gosu::Image(window->graphics(), name, false);
}

