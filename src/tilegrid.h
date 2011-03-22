/****************************
** Tsunagari Tile Engine   **
** tilegrid.h - TileGrid   **
** Copyright 2011 OmegaSDG **
****************************/

#include "resourcer.h"
#include "tile.h"

class TileGrid
{
public:
	TileGrid(Resourcer* rc, YAML* yaml);
	void draw();
	
	coord_t get_size();
	int get_tile_size();
	Tile* get_tile(coord_t coords);

private:
	vector<vector<Tile*>*> tiles;
};

