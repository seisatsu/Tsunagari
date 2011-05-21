/******************************
** Tsunagari Tile Engine     **
** tilematrix.h              **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef TILEMATRIX_H
#define TILEMATRIX_H

#include "common.h"
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
	typedef std::vector<Tile*> row_t;
	typedef std::vector<row_t> col_t;
	typedef std::vector<col_t> tilematrix_t;

	tilematrix_t matrix;
	Resourcer* rc;
	coord_t dim;
};

#endif

