/*********************************
** Tsunagari Tile Engine        **
** main.cpp                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <libxml/parser.h>

#include "client-conf.h"
#include "log.h"
#include "python.h"
#include "window.h"

#ifdef _WIN32
	#include <Windows.h>
	#include "os-windows.h"
#endif

#define ASSERT_MAIN(x)  if (!(x)) return 1

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
	#ifdef _WIN32
		wFixConsole();
	#endif

	parseConfig(CLIENT_CONF_PATH);

	ASSERT_MAIN(parseCommandLine(argc, argv));
	ASSERT_MAIN(conf.validate(CLIENT_CONF_PATH));

	Log::setVerbosity(conf.verbosity);
	ASSERT_MAIN(Log::init());

	std::cout << "[0.000] Starting " << TSUNAGARI_RELEASE_VERSION << std::endl;
	Log::reportVerbosityOnStartup();

	// Init various libraries we use.
	libraries libs;

	GameWindow window;
	ASSERT_MAIN(window.init(argv[0]));
	window.show();
	return 0;
}

