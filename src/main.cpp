/*********************************
** Tsunagari Tile Engine        **
** main.cpp                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <libxml/parser.h>

#include "client-conf.h"
#include "config.h"
#include "log.h"
#include "python.h"
#include "window.h"

#ifdef _WINDOWS
	#include <windows.h>
#endif

struct libraries
{
	libraries()
	{
		// Initialize the C library's random seed.
		srand((unsigned)time(NULL));

		/*
		 * This initializes the XML library and checks for potential
		 * ABI mismatches between the version it was compiled for and
		 * the actual shared library used.
		 */
		LIBXML_TEST_VERSION

		if (!pythonInit())
			exit(1);
	}

	~libraries()
	{
		pythonFinalize();
		xmlCleanupParser();
	}
};

/**
 * Load client config and instantiate window.
 *
 * The client config tells us our window parameters along with which World
 * we're going to load. The GameWindow class then loads and plays the game.
 */
int main(int argc, char** argv)
{
	#if _WINDOWS /* Fix console output on Windows */
	if (AttachConsole(ATTACH_PARENT_PROCESS)) {
		freopen("CONOUT$","wb",stdout);
		freopen("CONOUT$","wb",stderr);
	}
	#endif

	if (!parseConfig(CLIENT_CONF_FILE))
		return 1;
	if (!parseCommandLine(argc, argv))
		return 1;
	if (!conf.validate(CLIENT_CONF_FILE))
		return 1;
	if (conf.verbosity)
		Log::setVerbosity(conf.verbosity);

	// Init various libraries we use.
	libraries libs;

	GameWindow window;
	if (!window.init(argv[0]))
		return 1;
	window.show();
	return 0;
}

