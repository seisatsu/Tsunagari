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
	TileMatrix(Resourcer* rc);
	bool init();
	void draw();
	
	coord_t getDimensions();
	Tile* getTile(coord_t c);

private:
	Resourcer* rc;
	std::vector<std::vector<std::vector<Tile*> > > matrix;
	coord_t dim;
};

#endif

