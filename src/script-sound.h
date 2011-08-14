/******************************
** Tsunagari Tile Engine     **
** script-sound.h            **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef SCRIPT_SOUND_H
#define SCRIPT_SOUND_H

#include "script.h"

class Resourcer;

//! Give a script a 'Sound' object that can play sounds.
void bindSound(Script& script, Resourcer* rc);

#endif

