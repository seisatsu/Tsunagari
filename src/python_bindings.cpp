/****************************
** Tsunagari Tile Engine   **
** python_bindings.cpp     **
** Copyright 2012 OmegaSDG **
****************************/

#include <boost/python/module.hpp>

#include "audio.h" // for exportSound
#include "area.h" // for exportArea
#include "entity.h" // for exportEntity
#include "resourcer.h" // for exportResourcer
#include "tile.h" // for export{Tile,TileType,Door}
#include "vec.h" // for exportVecs

BOOST_PYTHON_MODULE(tsunagari)
{
	exportArea();
	exportDoor();
	exportEntity();
	exportResourcer();
	exportSound();
	exportTile();
	exportTileType();
	exportVecs();
}

// *sigh* If only it weren't static.
void pythonInitBindings()
{
	inittsunagari();
}

