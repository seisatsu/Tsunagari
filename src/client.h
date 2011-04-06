/******************************
** Tsunagari Tile Engine     **
** client.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <json/json.h>
#include <string>

#include "common.h"

struct ClientValues {
	std::string world;
	coord_t* windowsize;
	bool fullscreen;
};

ClientValues* __cconf;

