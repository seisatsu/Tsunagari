/*********************************
** Tsunagari Tile Engine        **
** common.cpp                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include "common.h"

Conf conf; // Project-wide global configuration.

icube_t icube(int x1, int y1, int z1,
              int x2, int y2, int z2)
{
	icube_t c;
	c.x1 = x1;
	c.y1 = y1;
	c.z1 = z1;
	c.x2 = x2;
	c.y2 = y2;
	c.z2 = z2;
	return c;
}

