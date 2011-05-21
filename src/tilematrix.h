/******************************
** Tsunagari Tile Engine     **
** tilegrid.h                **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef TILEGRID_H
#define TILEGRID_H

#include "common.h"
#include "resourcer.h"
#include "tile.h"

class Resourcer;
class Tile;

class TileGrid
{
public:
	TileGrid(Resourcer* rc, YAML* yaml);
	void draw();
	
	coord_t getSize();
	int getTileSize();
	Tile* getTile(coord_t coords);

private:
	vector<vector<vector<Tile*>*>*> grid;
};

#endif

