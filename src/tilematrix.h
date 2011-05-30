/******************************
** Tsunagari Tile Engine     **
** tilematrix.h              **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef TILEMATRIX_H
#define TILEMATRIX_H

#include <vector>

#include "common.h"

class Resourcer;
class Tile;

class TileMatrix
{
public:
	TileMatrix(Resourcer* rc);
	bool init();
	void draw();

	coord_t getDimensions() const;
	Tile* getTile(coord_t c) const;

private:
	typedef std::vector<Tile*> row_t;
	typedef std::vector<row_t> grid_t;
	typedef std::vector<grid_t> tilematrix_t;

	tilematrix_t matrix;
	Resourcer* rc;
	coord_t dim;
};

#endif

