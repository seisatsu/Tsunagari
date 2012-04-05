/****************************
** Tsunagari Tile Engine   **
** python_bindings.cpp     **
** Copyright 2012 OmegaSDG **
****************************/

#include <boost/python/module.hpp>

#include "area.h" // for exportArea
#include "entity.h" // for exportEntity
#include "music.h" // for exportMusic
#include "resourcer.h" // for exportResourcer
#include "sound.h" // for exportSound
#include "tile.h" // for export{Tile,TileType,Exit}
#include "vec.h" // for exportVecs

BOOST_PYTHON_MODULE(tsunagari)
{
	exportArea();
	exportEntity();
	exportExit();
	exportMusic();
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

