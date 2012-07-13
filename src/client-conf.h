/****************************
** Tsunagari Tile Engine   **
** client-conf.h           **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef CLIENT_CONF_H
#define CLIENT_CONF_H

#include <vector>

#include "log.h"
#include "vec.h"

//! Game Movement Mode
enum movement_mode_t {
	TURN,
	TILE,
	NOTILE
};

//! Engine-wide user-confurable values.
struct Conf {
	Conf();
	bool validate(const char* filename);

	std::string worldFilename;
	std::vector<std::string> dataPath;
	verbosity_t verbosity;
	movement_mode_t moveMode;
	icoord windowSize;
	bool fullscreen;
	bool audioEnabled;
	bool cacheEnabled;
	bool scriptHalt;
	int cacheTTL;
	int cacheSize;
	int persistInit;
	int persistCons;
};
extern Conf conf;

bool parseConfig(const char* filename);
bool parseCommandLine(int argc, char* argv[]);

#endif

