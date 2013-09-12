/***************************************
** Tsunagari Tile Engine              **
** main.cpp                           **
** Copyright 2011-2013 PariahSoft LLC **
***************************************/

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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <libxml/parser.h>

#include "client-conf.h"
#include "log.h"
#include "python.h"
#include "reader.h"
#include "window.h"

#ifdef _WIN32
	#include "os-windows.h"
#endif

#ifdef __APPLE__
	#include "os-mac.h"
#endif

#define ASSERT_RETURN1(x)  if (!(x)) { return 1; }

struct libraries
{
	libraries(char* argv0)
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

		// Reader::init runs Python scripts.
		if (!Reader::init(argv0))
			exit(1);
	}

	~libraries()
	{
		Reader::deinit();
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
	
	#ifdef __APPLE__
		macSetWorkingDirectory();
	#endif

	ASSERT_RETURN1(Log::init());

	parseConfig(CLIENT_CONF_PATH);
	ASSERT_RETURN1(parseCommandLine(argc, argv));
	ASSERT_RETURN1(conf.validate(CLIENT_CONF_PATH));

	std::cout << "[0.000] Starting " << TSUNAGARI_RELEASE_VERSION << std::endl;

	Log::setVerbosity(conf.verbosity);
	Log::reportVerbosityOnStartup();

	GameWindow window;

	// Init various libraries we use.
	libraries libs(argv[0]);

	ASSERT_RETURN1(window.init());
	window.show();
	return 0;
}

