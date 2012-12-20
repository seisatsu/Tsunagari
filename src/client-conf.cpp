/*********************************
** Tsunagari Tile Engine        **
** client-conf.cpp              **
** Copyright 2011-2012 OmegaSDG **
*********************************/

// "OmegaSDG" is defined as Michael D. Reiley and Paul Merrill.

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
#include <fstream>

#include <boost/config.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "client-conf.h"
#include "nbcl/nbcl.h"
#include "string.h"

Conf conf; // Project-wide global configuration.

// Initialize and set configuration defaults.
Conf::Conf()
{
	persistInit = 0;
	persistCons = 0;
}

bool Conf::validate(const std::string& filename)
{
	if (conf.worldFilename == "") {
		Log::fatal(filename, "\"[engine] world\" option or equivalent command line option expected");
		return false;
	}
	return true;
}

/* Output compiled-in engine defaults. */
static void defaultsQuery()
{
	std::cerr << "CLIENT_CONF_PATH:                    "
		<< CLIENT_CONF_PATH << std::endl;
	std::cerr << "BASE_ZIP_PATH:                       "
		<< BASE_ZIP_PATH << std::endl;
	std::cerr << "DEF_ENGINE_VERBOSITY:                "
		<< DEF_ENGINE_VERBOSITY << std::endl;
	std::cerr << "DEF_ENGINE_HALTING:                  "
		<< DEF_ENGINE_HALTING << std::endl;
	std::cerr << "DEF_WINDOW_WIDTH:                    "
		<< DEF_WINDOW_WIDTH << std::endl;
	std::cerr << "DEF_WINDOW_HEIGHT:                   "
		<< DEF_WINDOW_HEIGHT << std::endl;
	std::cerr << "DEF_WINDOW_FULLSCREEN:               "
		<< DEF_WINDOW_FULLSCREEN << std::endl;
	std::cerr << "DEF_CACHE_ENABLED:                   "
		<< DEF_CACHE_ENABLED << std::endl;
	std::cerr << "DEF_CACHE_TTL:                       "
		<< DEF_CACHE_TTL << std::endl;
	std::cerr << "DEF_CACHE_SIZE:                      "
		<< DEF_CACHE_SIZE << std::endl;
}

// Parse and process the client config file, and set configuration defaults for
// missing options.
bool parseConfig(const std::string& filename)
{
	namespace pt = boost::property_tree;
	pt::ptree ini;
	
	bool parse_error = false;

	conf.cacheEnabled = DEF_CACHE_TTL && DEF_CACHE_SIZE;

	try
	{
		pt::read_ini(filename.c_str(), ini);
	}
	catch (pt::ini_parser_error)
	{
		Log::err(filename, "could not parse config");
		parse_error = true;
	}

	conf.worldFilename = ini.get("engine.world", "");
	conf.dataPath = splitStr(ini.get("engine.datapath", ""), ",");
	conf.windowSize.x = ini.get("window.width", DEF_WINDOW_WIDTH);
	conf.windowSize.y = ini.get("window.height", DEF_WINDOW_HEIGHT);
	conf.fullscreen = ini.get("window.fullscreen", DEF_WINDOW_FULLSCREEN);
	conf.audioEnabled = ini.get("audio.enabled", true);
	conf.cacheEnabled = ini.get("cache.enabled", DEF_CACHE_ENABLED);

	conf.musicVolume = ini.get("audio.musicvolume", 100);
	if (conf.musicVolume < 0)
		conf.musicVolume = 0;
	else if (conf.musicVolume > 100)
		conf.musicVolume = 100;

	conf.soundVolume = ini.get("audio.soundvolume", 100);
	if (conf.soundVolume < 0)
		conf.soundVolume = 0;
	else if (conf.soundVolume > 100)
		conf.soundVolume = 100;

	conf.cacheTTL = ini.get("cache.ttl", DEF_CACHE_TTL);
	if (!conf.cacheTTL)
		conf.cacheEnabled = false;

	conf.cacheSize = ini.get("cache.size", DEF_CACHE_SIZE);
	if (!conf.cacheSize)
		conf.cacheEnabled = false;

	std::string verbosity = ini.get("engine.verbosity", DEF_ENGINE_VERBOSITY);
	if (verbosity.empty())
		;
	else if (verbosity == "quiet")
		conf.verbosity = V_QUIET;
	else if (verbosity == "normal")
		conf.verbosity = V_NORMAL;
	else if (verbosity == "verbose")
		conf.verbosity = V_VERBOSE;
	else {
		Log::err(filename, "unknown value for \"[engine] verbosity\", using default");
	}

	std::string halting = ini.get("engine.halting", DEF_ENGINE_HALTING);
	if (halting.empty())
		;
	else if (halting == "fatal")
		conf.halting = HALT_FATAL;
	else if (halting == "script")
		conf.halting = HALT_SCRIPT;
	else if (halting == "error")
		conf.halting = HALT_ERROR;
	else {
		Log::err(filename, "unknown value for \"[engine] halting\", using default");
	}

	if (parse_error)
		return false;
	return true;
}

// Parse and process command line options and arguments.
bool parseCommandLine(int argc, char* argv[])
{
	NBCL cmd(argc, argv);

	cmd.setStrayArgsDesc("[WORLD FILE]");

	cmd.insert("-h", "--help",         "",                "Display this help message");
	cmd.insert("-c", "--config",       "<config file>",   "Client config file to use");
	cmd.insert("-p", "--datapath",     "<file,file,...>", "Prepend zips to data path");
	cmd.insert("-q", "--quiet",        "",                "Display only fatal errors");
	cmd.insert("-n", "--normal",       "",                "Display all errors");
	cmd.insert("-v", "--verbose",      "",                "Display additional information");
	cmd.insert("-t", "--cache-ttl",    "<seconds>",       "Cache time-to-live in seconds");
	cmd.insert("-m", "--cache-size",   "<megabytes>",     "Cache size in megabytes");
	cmd.insert("-s", "--size",         "<WxH>",           "Window dimensions");
	cmd.insert("-f", "--fullscreen",   "",                "Run in fullscreen mode");
	cmd.insert("-w", "--window",       "",                "Run in windowed mode");
	cmd.insert("",   "--halt-fatal",   "",                "Stop engine only on fatal errors");
	cmd.insert("",   "--halt-script",  "",                "Stop engine on script errors");
	cmd.insert("",   "--halt-error",   "",                "Stop engine on all errors");
	cmd.insert("",   "--no-audio",     "",                "Disable audio");
	cmd.insert("",   "--volume-music", "<0-100>",         "Set music volume");
	cmd.insert("",   "--volume-sound", "<0-100>",         "Set sound effects volume");
	cmd.insert("",   "--query",        "",                "Query compiled-in engine defaults");
	cmd.insert("",   "--version",      "",                "Print the engine version string");

	if (!cmd.parse()) {
		cmd.usage();
		return false;
	}

	std::vector<std::string> strayArgs = cmd.getStrayArgsList();
	if (strayArgs.size() > 1) {
		cmd.usage();
		return false;
	}
	else if (strayArgs.size() == 1)
		conf.worldFilename = strayArgs[0];

	if (cmd.check("--help")) {
		cmd.usage();
		return false;
	}

	if (cmd.check("--version")) {
		std::cout << TSUNAGARI_RELEASE_VERSION << std::endl;
		return false;
	}

	if (cmd.check("--query")) {
		defaultsQuery();
		return false;
	}

	if (cmd.check("--config")) {
		if (!parseConfig(cmd.get("--config")))
			return false;
	}

	if (cmd.check("--datapath"))
		conf.dataPath = splitStr(cmd.get("--datapath"), ",");

	int verbcount = 0;
	if (cmd.check("--quiet")) {
		conf.verbosity = V_QUIET;
		verbcount++;
	}
	if (cmd.check("--normal")) {
		conf.verbosity = V_NORMAL;
		verbcount++;
	}
	if (cmd.check("--verbose")) {
		conf.verbosity = V_VERBOSE;
		verbcount++;
	}
	if (verbcount > 1)
		Log::err("cmdline", "multiple verbosity flags on cmdline, using most verbose");

	int haltcount = 0;
	if (cmd.check("--halt-fatal")) {
		conf.halting = HALT_FATAL;
		haltcount++;
	}
	if (cmd.check("--halt-script")) {
		conf.halting = HALT_SCRIPT;
		haltcount++;
	}
	if (cmd.check("--halt-error")) {
		conf.halting = HALT_ERROR;
		haltcount++;
	}
	if (haltcount > 1)
		Log::err("cmdline", "multiple halting flags on cmdline, using most stringent");

	if (cmd.check("--no-audio"))
		conf.audioEnabled = false;

	if (cmd.check("--volume-music"))
		conf.musicVolume = parseInt100(cmd.get("--volume-music"));

	if (cmd.check("--volume-sound"))
		conf.soundVolume = parseInt100(cmd.get("--volume-sound"));

	if (cmd.check("--cache-ttl")) {
		conf.cacheTTL = parseUInt(cmd.get("--cache-ttl"));
		if (conf.cacheTTL == 0)
			conf.cacheEnabled = false;
	}

	if (cmd.check("--cache-size")) {
		conf.cacheSize = parseUInt(cmd.get("--cache-size"));
		if (conf.cacheSize == 0)
			conf.cacheEnabled = false;
	}

	if (cmd.check("--size")) {
		std::vector<std::string> dim = splitStr(cmd.get("--size"), "x");
		if (dim.size() != 2) {
			Log::fatal("cmdline", "invalid argument for -s/--size");
			return false;
		}
		conf.windowSize.x = parseUInt(dim[0]);
		conf.windowSize.y = parseUInt(dim[1]);
	}

	if (cmd.check("--fullscreen") && cmd.check("--window")) {
		Log::fatal("cmdline", "-f/--fullscreen and -w/--window mutually exclusive");
		return false;
	}

	if (cmd.check("--fullscreen"))
		conf.fullscreen = true;

	if (cmd.check("--window"))
		conf.fullscreen = false;

	return true;
}

