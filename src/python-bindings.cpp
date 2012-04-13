/****************************
** Tsunagari Tile Engine   **
** python-bindings.cpp     **
** Copyright 2012 OmegaSDG **
****************************/

#include <boost/python/module.hpp>

#include "area.h"
#include "entity.h"
#include "music.h"
#include "random.h"
#include "resourcer.h"
#include "sound.h"
#include "tile.h"
#include "timer.h"
#include "timeout.h"
#include "vec.h"

BOOST_PYTHON_MODULE(tsunagari)
{
	exportArea();
	exportEntity();
	exportMusic();
	exportRandom();
	exportResourcer();
	exportSound();
	exportTile();
	exportTimeout();
	exportTimer();
	exportVecs();
}

// *sigh* If only it weren't static.
void pythonInitBindings()
{
	inittsunagari();
}

