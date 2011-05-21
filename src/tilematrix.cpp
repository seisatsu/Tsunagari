/******************************
** Tsunagari Tile Engine     **
** tilematrix.cpp            **
** Copyright 2011 OmegaSDG   **
******************************/

#include <boost/foreach.hpp>

#include "tilematrix.h"

TileMatrix::TileMatrix(Resourcer* rc) : rc(rc)
{
}

bool TileMatrix::init()
{
	Sprite* s = new Sprite(rc, "grass.sprite");
	Sprite* s2 = new Sprite(rc, "grass.sprite");
	if (!s->init() || !s2->init())
		return false;
	Tile* t = new Tile(s, true, coord(0, 0, 0));
	Tile* t2 = new Tile(s2, true, coord(1, 0, 0));


	matrix.resize(1);
	matrix[0].resize(1);
	matrix[0][0].resize(2);
	matrix[0][0][0] = t;
	matrix[0][0][1] = t2;
	return true;
}

void TileMatrix::draw()
{
	BOOST_FOREACH(grid_t g, matrix)
		BOOST_FOREACH(row_t r, g)
			BOOST_FOREACH(Tile* t, r)
				t->draw();
}

coord_t TileMatrix::getDimensions()
{
	return dim;
}

Tile* TileMatrix::getTile(coord_t c)
{
	return matrix[c.z][c.y][c.x];
}

