/****************************
** Tsunagari Tile Engine   **
** client-conf.cpp         **
** Copyright 2012 OmegaSDG **
****************************/

#include <iostream>
#include <fstream>

#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>

#include "client-conf.h"
#include "cmd.h"
#include "config.h"
#include "string.h"

Conf conf; // Project-wide global configuration.

Conf::Conf()
{
	worldFilename = "";
	windowSize.x = 0;
	windowSize.y = 0;
	persistInit = 0;
	persistCons = 0;
}

// Check for missing required configuration variables.
bool Conf::validate(const char* filename)
{
	bool good_conf = true;

	if (conf.worldFilename == "") {
		Log::fatal(filename, "\"[engine] world\" option or equivalent command line option expected");
		good_conf = false;
	}
	if (!conf.windowSize.x) {
		Log::fatal(filename, "\"[window] width\" option or equivalent command line option expected");
		good_conf = false;
	}
	if (!conf.windowSize.y) {
		Log::fatal(filename, "\"[window] height\" option or equivalent command line option expected");
		good_conf = false;
	}
	if (!conf.fullscreen_opt_set) {
		Log::fatal(filename, "\"[window] fullscreen\" option or equivalent command line option expected");
		good_conf = false;
	}
	if (good_conf)
		return true;
	else
		return false;
}

/* Output compiled-in engine defaults. */
static void defaultsQuery()
{
	std::cerr << "CLIENT_CONF_FILE:                       "
		<< CLIENT_CONF_FILE << std::endl;
	std::cerr << "DTD_DIRECTORY:                          "
		<< DTD_DIRECTORY << std::endl;
	std::cerr << "CACHE_EMPTY_TTL:                        "
		<< CACHE_EMPTY_TTL << std::endl;
	std::cerr << "CACHE_MAX_SIZE:                         "
		<< CACHE_MAX_SIZE << std::endl;
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

	conf.cacheEnabled = CACHE_EMPTY_TTL && CACHE_MAX_SIZE;

	std::ifstream config(filename);
	if (!config) {
		Log::fatal(filename, "could not parse config");
		return false;
	}

	std::set<std::string> options;
	std::map<std::string, std::string> parameters;
	options.insert("*");

	for (pod::config_file_iterator i(config, options), e ; i != e; ++i)
		parameters[i->string_key] = i->value[0];

	if (!parameters["engine.world"].empty())
		conf.worldFilename = parameters["engine.world"];

	if (!parameters["window.width"].empty())
		conf.windowSize.x = atoi(parameters["window.width"].c_str());

	if (!parameters["window.height"].empty())
		conf.windowSize.y = atoi(parameters["window.height"].c_str());

	if (!parameters["window.fullscreen"].empty()) {
		conf.fullscreen_opt_set = true;
		conf.fullscreen = parseBool(parameters["window.fullscreen"]);
	}

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
		conf.cacheTTL = CACHE_EMPTY_TTL;
	else {
		if (atoi(parameters["cache.ttl"].c_str()) == 0)
			conf.cacheEnabled = 0;
		conf.cacheTTL = atoi(parameters["cache.ttl"].c_str());
	}

	if (parameters["cache.size"].empty())
		conf.cacheSize = CACHE_MAX_SIZE;
	else {
		if (atoi(parameters["cache.size"].c_str()) == 0)
			conf.cacheEnabled = 0;
		conf.cacheSize = atoi(parameters["cache.size"].c_str());
	}

	std::string verbosity = parameters["engine.verbosity"];
	conf.verbosity = V_NORMAL;
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

	return true;
}

/* Parse and process command line options and arguments. */
bool parseCommandLine(int argc, char* argv[])
{
	CommandLineOptions cmd(argc, argv);

	cmd.insert("-h", "--help",       "",              "Display this help message");
	cmd.insert("-g", "--gameworld",  "<world file>",  "Game world to load");
	cmd.insert("-c", "--config",     "<config file>", "Client config file to use");
	cmd.insert("-q", "--quiet",      "",              "Display only fatal errors");
	cmd.insert("",   "--normal",     "",              "Display all errors");
	cmd.insert("-v", "--verbose",    "",              "Display additional information");
	cmd.insert("-t", "--cache-ttl",  "<seconds>",     "Cache time-to-live in seconds");
	cmd.insert("-m", "--cache-size", "<megabytes>",   "Cache size in megabytes");
	cmd.insert("-s", "--size",       "<WxH>",         "Window dimensions");
	cmd.insert("-f", "--fullscreen", "",              "Run in fullscreen mode");
	cmd.insert("-w", "--window",     "",              "Run in windowed mode");
	cmd.insert("",   "--no-audio",   "",              "Disable audio");
	cmd.insert("",   "--query",      "",              "Query compiled-in engine defaults");
	cmd.insert("",   "--version",    "",              "Print the engine version string");

	if (!cmd.parse()) {
		cmd.usage();
		return false;
	}

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

	if (cmd.check("--gameworld"))
		conf.worldFilename = cmd.get("--gameworld");

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

	if (cmd.check("--fullscreen")) {
		conf.fullscreen = true;
		conf.fullscreen_opt_set = true;
	}

	if (cmd.check("--window")) {
		conf.fullscreen = false;
		conf.fullscreen_opt_set = true;
	}

	return true;
}

