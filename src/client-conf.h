/****************************
** Tsunagari Tile Engine   **
** client-conf.h           **
** Copyright 2012 OmegaSDG **
****************************/

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to 
// deal in the Software without restriction, including without limitation the 
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
// IN THE SOFTWARE.
// **********

#ifndef CLIENT_CONF_H
#define CLIENT_CONF_H

#include <string>
#include <vector>

#include "log.h"
#include "vec.h"

/* Release version. */
#define TSUNAGARI_RELEASE_VERSION "Tsunagari Tile Engine AlphaP4 Revision 4"

// === Required Data Paths ===
	/* Tsunagari config file. */
	#define CLIENT_CONF_PATH "./client.ini"

	/* Base data file. */
	#define BASE_ZIP_PATH "./data/base.zip"
// ===

// === Client.ini Default Values ===
	#define DEF_ENGINE_VERBOSITY  "verbose"
	#define DEF_ENGINE_HALTING    "fatal"
	#define DEF_WINDOW_WIDTH      640
	#define DEF_WINDOW_HEIGHT     480
	#define DEF_WINDOW_FULLSCREEN false
	#define DEF_AUDIO_ENABLED     true
	#define DEF_CACHE_ENABLED     true
	#define DEF_CACHE_TTL         300
	#define DEF_CACHE_SIZE        100
// ===

//! Game Movement Mode
enum movement_mode_t {
	TURN,
	TILE,
	NOTILE
};

//! Halting Mode
enum halting_mode_t {
	HALT_FATAL,
	HALT_SCRIPT,
	HALT_ERROR
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
	halting_mode_t halting;
	icoord windowSize;
	bool fullscreen;
	bool audioEnabled;
	bool cacheEnabled;
	int cacheTTL;
	int cacheSize;
	int persistInit;
	int persistCons;
};
extern Conf conf;

bool parseConfig(const char* filename);
bool parseCommandLine(int argc, char* argv[]);

#endif

