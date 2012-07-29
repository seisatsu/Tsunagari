/****************************
** Tsunagari Tile Engine   **
** client-conf.cpp         **
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

#include <iostream>
#include <fstream>

#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>

#include "client-conf.h"
#include "nbcl/nbcl.h"
#include "string.h"

Conf conf; // Project-wide global configuration.

Conf::Conf()
{
	worldFilename = "";
	if (!strcmp(DEF_ENGINE_VERBOSITY, "quiet"))
		conf.verbosity = V_QUIET;
	else if (!strcmp(DEF_ENGINE_VERBOSITY, "normal"))
		conf.verbosity = V_NORMAL;
	else if (!strcmp(DEF_ENGINE_VERBOSITY, "verbose"))
		conf.verbosity = V_VERBOSE;
	if (!strcmp(DEF_ENGINE_HALTING, "fatal"))
		conf.halting = FATAL;
	else if (!strcmp(DEF_ENGINE_HALTING, "script"))
		conf.halting = SCRIPT;
	else if (!strcmp(DEF_ENGINE_HALTING, "error"))
		conf.halting = ERROR;
	windowSize.x = DEF_WINDOW_WIDTH;
	windowSize.y = DEF_WINDOW_HEIGHT;
	fullscreen = DEF_WINDOW_FULLSCREEN;
	audioEnabled = DEF_AUDIO_ENABLED;
	cacheEnabled = DEF_CACHE_ENABLED;
	cacheTTL = DEF_CACHE_TTL;
	cacheSize = DEF_CACHE_SIZE;
	persistInit = 0;
	persistCons = 0;
}

bool Conf::validate(const char* filename)
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
	std::cerr << "XML_DTD_PATH:                        "
		<< XML_DTD_PATH << std::endl;
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
	std::cerr << "DEF_AUDIO_ENABLED:                   "
		<< DEF_AUDIO_ENABLED << std::endl;
	std::cerr << "DEF_CACHE_ENABLED:                   "
		<< DEF_CACHE_ENABLED << std::endl;
	std::cerr << "DEF_CACHE_TTL:                       "
		<< DEF_CACHE_TTL << std::endl;
	std::cerr << "DEF_CACHE_SIZE:                      "
		<< DEF_CACHE_SIZE << std::endl;
}

/**
 * Load the values we need to start initializing the game from an ini file.
 *
 * We need to know what size window to create and which World to load. This
 * information will be stored in an ini file which we parse here.
 *
 * @param filename Name of the ini file to load from.
 *
 * @return false if error occured during processing
 */
bool parseConfig(const char* filename)
{
	namespace pod = boost::program_options::detail;

	conf.cacheEnabled = DEF_CACHE_TTL && DEF_CACHE_SIZE;

	std::ifstream config(filename);
	if (!config) {
		Log::err(filename, "could not parse config");
		return false;
	}

	std::set<std::string> options;
	std::map<std::string, std::string> parameters;
	options.insert("*");

	for (pod::config_file_iterator i(config, options), e ; i != e; ++i)
		parameters[i->string_key] = i->value[0];

	if (!parameters["engine.world"].empty())
		conf.worldFilename = parameters["engine.world"];

	if (!parameters["engine.datapath"].empty())
		conf.dataPath = splitStr(parameters["engine.datapath"], ",");

	if (!parameters["window.width"].empty())
		conf.windowSize.x = atoi(parameters["window.width"].c_str());

	if (!parameters["window.height"].empty())
		conf.windowSize.y = atoi(parameters["window.height"].c_str());

	if (!parameters["window.fullscreen"].empty())
		conf.fullscreen = parseBool(parameters["window.fullscreen"]);

	if (parameters["audio.enabled"].size())
		conf.audioEnabled = parseBool(parameters["audio.enabled"]);
	else
		conf.audioEnabled = true;

	if (!parameters["cache.enabled"].empty()) {
		if (parseBool(parameters["cache.enabled"]))
			conf.cacheEnabled = true;
		else
			conf.cacheEnabled = false;
	}

	if (parameters["cache.ttl"].empty())
		conf.cacheTTL = DEF_CACHE_TTL;
	else {
		if (atoi(parameters["cache.ttl"].c_str()) == 0)
			conf.cacheEnabled = 0;
		conf.cacheTTL = atoi(parameters["cache.ttl"].c_str());
	}

	if (parameters["cache.size"].empty())
		conf.cacheSize = DEF_CACHE_SIZE;
	else {
		if (atoi(parameters["cache.size"].c_str()) == 0)
			conf.cacheEnabled = 0;
		conf.cacheSize = atoi(parameters["cache.size"].c_str());
	}

	std::string verbosity = parameters["engine.verbosity"];
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

	std::string halting = parameters["engine.halting"];
	if (halting.empty())
		;
	else if (halting == "fatal")
		conf.halting = FATAL;
	else if (halting == "script")
		conf.halting = SCRIPT;
	else if (halting == "error")
		conf.halting = ERROR;
	else {
		Log::err(filename, "unknown value for \"[engine] halting\", using default");
	}

	return true;
}

//! Parse and process command line options and arguments.
bool parseCommandLine(int argc, char* argv[])
{
	NBCL cmd(argc, argv);

	cmd.setStrayArgsDesc("[WORLD FILE]");

	cmd.insert("-h", "--help",        "",                "Display this help message");
	cmd.insert("-c", "--config",      "<config file>",   "Client config file to use");
	cmd.insert("-p", "--datapath",    "<file,file,...>", "Prepend zips to data path");
	cmd.insert("-q", "--quiet",       "",                "Display only fatal errors");
	cmd.insert("",   "--normal",      "",                "Display all errors");
	cmd.insert("-v", "--verbose",     "",                "Display additional information");
	cmd.insert("-t", "--cache-ttl",   "<seconds>",       "Cache time-to-live in seconds");
	cmd.insert("-m", "--cache-size",  "<megabytes>",     "Cache size in megabytes");
	cmd.insert("-s", "--size",        "<WxH>",           "Window dimensions");
	cmd.insert("-f", "--fullscreen",  "",                "Run in fullscreen mode");
	cmd.insert("-w", "--window",      "",                "Run in windowed mode");
	cmd.insert("",   "--fatal-halt",  "",                "Stop engine only on fatal errors");
	cmd.insert("",   "--script-halt", "",                "Stop engine on script errors");
	cmd.insert("",   "--error-halt",  "",                "Stop engine on all errors");
	cmd.insert("",   "--no-audio",    "",                "Disable audio");
	cmd.insert("",   "--query",       "",                "Query compiled-in engine defaults");
	cmd.insert("",   "--version",     "",                "Print the engine version string");

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
		if (!parseConfig(cmd.get("--config").c_str()))
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
	if (cmd.check("--fatal-halt")) {
		conf.halting = FATAL;
		haltcount++;
	}
	if (cmd.check("--script-halt")) {
		conf.halting = SCRIPT;
		haltcount++;
	}
	if (cmd.check("--error-halt")) {
		conf.halting = ERROR;
		haltcount++;
	}
	if (haltcount > 1)
		Log::err("cmdline", "multiple halting flags on cmdline, using most stringent");

	if (cmd.check("--no-audio"))
		conf.audioEnabled = false;

	if (cmd.check("--cache-ttl")) {
		conf.cacheTTL = atoi(cmd.get("--cache-ttl").c_str());
		if (conf.cacheTTL == 0)
			conf.cacheEnabled = false;
	}

	if (cmd.check("--cache-size")) {
		conf.cacheSize = atoi(cmd.get("--cache-size").c_str());
		if (conf.cacheSize == 0)
			conf.cacheEnabled = false;
	}

	if (cmd.check("--size")) {
		std::vector<std::string> dim = splitStr(cmd.get("--size"), "x");
		if (dim.size() != 2) {
			Log::fatal("cmdline", "invalid argument for -s/--size");
			return false;
		}
		conf.windowSize.x = atoi(dim[0].c_str());
		conf.windowSize.y = atoi(dim[1].c_str());
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

