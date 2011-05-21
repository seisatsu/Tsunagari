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

