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
#include "tile.h" // for exportTile
#include "vec.h" // for exportVecs

#include "python_random.h" // for exportRandom
#include "python_timer.h" // for exportTimer

BOOST_PYTHON_MODULE(tsunagari)
{
	exportArea();
	exportEntity();
	exportMusic();
	exportResourcer();
	exportSound();
	exportTile();
	exportVecs();
	exportRandom();
	exportTimer();
}

// *sigh* If only it weren't static.
void pythonInitBindings()
{
	inittsunagari();
}

