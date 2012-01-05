/****************************
** Tsunagari Tile Engine   **
** python_bindings.cpp     **
** Copyright 2012 OmegaSDG **
****************************/

#include <boost/python/module.hpp>

#include "area.h" // for exportArea
#include "entity.h" // for exportEntity
#include "math.h" // for exportVecs
#include "resourcer.h" // for exportResourcer
#include "sound.h" // for exportSound
#include "tile.h" // for export{Tile,TileType,Door}

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

