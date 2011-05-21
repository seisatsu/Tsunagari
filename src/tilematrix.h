/******************************
** Tsunagari Tile Engine     **
** tilematrix.h              **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef TILEMATRIX_H
#define TILEMATRIX_H

#include "common.h"
#include "resourcer.h"
#include "tile.h"

class Resourcer;
class Tile;

class TileMatrix
{
public:
	TileMatrix(Resourcer* rc;
	void draw();
	
	coord_t getDimensions();
	Tile* getTile(coord_t coords);

private:
	std::vector<std::vector<std::vector<Tile*>*>*> grid;
};

#endif

