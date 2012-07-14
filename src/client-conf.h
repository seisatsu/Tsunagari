/****************************
** Tsunagari Tile Engine   **
** client-conf.h           **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef CLIENT_CONF_H
#define CLIENT_CONF_H

#include <string>
#include <vector>

#include "log.h"
#include "vec.h"

/* Release version. */
#define TSUNAGARI_RELEASE_VERSION "Tsunagari Tile Engine AlphaP4 Revision 3"

// === Required Data Paths ===
	/* Tsunagari config file. */
	#define CLIENT_CONF_PATH "./client.ini"

	/* Base data file. */
	#define BASE_ZIP_PATH "../data/base.zip"

	/* Tsunagari XML DTD directory. */
	#define XML_DTD_PATH "../data/dtd/"
// ===

// === Client.ini Default Values ===
	#define DEF_ENGINE_VERBOSITY	"verbose"
	#define DEF_WINDOW_WIDTH		640
	#define DEF_WINDOW_HEIGHT		480
	#define DEF_WINDOW_FULLSCREEN	false
	#define DEF_AUDIO_ENABLED		true
	#define DEF_CACHE_ENABLED		true
	#define DEF_CACHE_TTL			300
	#define DEF_CACHE_SIZE			100
// ===

//! Game Movement Mode
enum movement_mode_t {
	TURN,
	TILE,
	NOTILE
};

//! Engine-wide user-confurable values.
struct Conf {
	Conf();

	/**
	 * Check for missing required configuration variables.
	 */
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

