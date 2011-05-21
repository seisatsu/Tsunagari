/******************************
** Tsunagari Tile Engine     **
** tilematrix.cpp            **
** Copyright 2011 OmegaSDG   **
******************************/

#include "tilematrix.h"

TileMatrix::TileMatrix(Resourcer* rc) : rc(rc)
{
}

bool TileMatrix::init()
{
	Sprite* s = new Sprite(rc, "grass.sprite");
	if (!s->init())
		return false;
	Tile* t = new Tile(s, true, {0, 0, 0});


	matrix.resize(1);
	matrix[0].resize(1);
	matrix[0][0].resize(1);
	matrix[0][0][0] = t;
	return true;
}

void TileMatrix::draw()
{
}

coord_t TileMatrix::getDimensions()
{
	return dim;
}

Tile* TileMatrix::getTile(coord_t c)
{
	return matrix[c.x][c.y][c.z];
}

