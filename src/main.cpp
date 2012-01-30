/*********************************
** Tsunagari Tile Engine        **
** main.cpp                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <time.h>

#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <libxml/parser.h>
#include <boost/scoped_ptr.hpp>

#include "common.h"
#include "config.h"
#include "cmd.h"
#include "log.h"
#include "python.h"
#include "window.h"

#ifdef _WINDOWS
	#include <windows.h>
#endif

/* Output compiled-in engine defaults. */
static void defaultsQuery()
{
	std::cerr << "CLIENT_CONF_FILE:                       "
		<< CLIENT_CONF_FILE << std::endl;
	std::cerr << "DTD_DIRECTORY:                          "
		<< DTD_DIRECTORY << std::endl;
	std::cerr << "MESSAGE_MODE:                           ";
	if (MESSAGE_MODE == MM_DEBUG)
		std::cerr << "MM_DEBUG" << std::endl;
	else if (MESSAGE_MODE == MM_DEVELOPER)
		std::cerr << "MM_DEVELOPER" << std::endl;
	else
		std::cerr << "MM_ERROR" << std::endl;
	std::cerr << "ROGUELIKE_PERSIST_DELAY_INIT:           "
		<< ROGUELIKE_PERSIST_DELAY_INIT << std::endl;
	std::cerr << "ROGUELIKE_PERSIST_DELAY_CONSECUTIVE:    "
		<< ROGUELIKE_PERSIST_DELAY_CONSECUTIVE << std::endl;
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
static bool parseConfig(const char* filename)
{
	namespace pod = boost::program_options::detail;

	conf.cacheEnabled = CACHE_EMPTY_TTL && CACHE_MAX_SIZE;

	std::ifstream config(filename);
	if (!config) {
		Log::err(filename, "could not parse config");
		return false;
	}

	std::set<std::string> options;
	std::map<std::string, std::string> parameters;
	options.insert("*");

	for (pod::config_file_iterator i(config, options), e ; i != e; ++i) {
		parameters[i->string_key] = i->value[0];
	}

	if (parameters["engine.world"].empty()) {
		Log::err(filename, "\"[engine] world\" option expected");
		return false;
	}
	else
		conf.world = parameters["engine.world"];

	if (parameters["window.width"].empty()) {
		Log::err(filename, "\"[window] width\" option expected");
		return false;
	}
	else
		conf.windowSize.x = atoi(parameters["window.width"].c_str());

	if (parameters["window.height"].empty()) {
		Log::err(filename, "\"[window] height\" option expected");
		return false;
	}
	else
		conf.windowSize.y = atoi(parameters["window.height"].c_str());

	if (parameters["window.fullscreen"].empty()) {
		Log::err(filename, "\"[window] fullscreen\" option expected");
		return false;
	}
	else
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

	if (parameters["engine.loglevel"].empty())
		conf.logLevel = MESSAGE_MODE;
	else if (parameters["engine.loglevel"] == "error" ||
	    parameters["engine.loglevel"] == "Error" ||
	    parameters["engine.loglevel"] == "ERROR")
		conf.logLevel = MM_SILENT;
	else if (parameters["engine.loglevel"] == "devel" ||
	    parameters["engine.loglevel"] == "Devel" ||
	    parameters["engine.loglevel"] == "DEVEL")
	conf.logLevel = MM_DEVELOPER;
	else if (parameters["engine.loglevel"] == "debug" ||
	    parameters["engine.loglevel"] == "Debug" ||
	    parameters["engine.loglevel"] == "DEBUG")
	conf.logLevel = MM_DEBUG;
	else {
		Log::err(filename, "unknown value for \"[engine] loglevel\", using default");
		conf.logLevel = MESSAGE_MODE;
	}

	return true;
}

/* Parse and process command line options and arguments. */
static bool parseCommandLine(int argc, char* argv[])
{
	CommandLineOptions cmd(argc, argv);

	cmd.insert("-h", "--help", "", "Display this help message");
	cmd.insert("-g", "--gameworld", "<world file>", "Game world to load");
	cmd.insert("-c", "--config", "<config file>", "Client config file to use");
	cmd.insert("-v", "--verbosity", "<error,devel,debug>", "Log message level");
	cmd.insert("-t", "--cache-ttl", "<seconds>", "Cache time-to-live in seconds");
	cmd.insert("-m", "--cache-size", "<megabytes>", "Cache size in megabytes");
	cmd.insert("-s", "--size", "<WxH>", "Window dimensions");
	cmd.insert("-f", "--fullscreen", "", "Run in fullscreen mode");
	cmd.insert("-w", "--window", "", "Run in windowed mode");
	cmd.insert("", "--no-audio", "", "Disable audio");
	cmd.insert("-q", "--query", "", "Query compiled-in engine defaults");
	cmd.insert("", "--version", "", "Print the engine version string");

	if (!cmd.parse()) {
		Log::err(argv[0], "bad command line");
		cmd.usage();
		return false;
	}

	if (cmd.check("--help")) {
		cmd.usage();
		return false;
	}

	if (cmd.check("--version")) {
		std::cerr << TSUNAGARI_RELEASE_VERSION << std::endl;
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
		conf.world = cmd.get("--gameworld");

	if (cmd.check("--verbosity")) {
		if (cmd.get("--verbosity") == "error")
			conf.logLevel = MM_SILENT;
		else if (cmd.get("--verbosity") == "devel")
			conf.logLevel = MM_DEVELOPER;
		else if (cmd.get("--verbosity") == "debug")
			conf.logLevel = MM_DEBUG;
		else {
			Log::err(argv[0], "invalid argument for --verbosity");
			return false;
		}
	}

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
			Log::err(argv[0], "invalid argument for --size");
			return false;
		}
		conf.windowSize.x = atoi(dim[0].c_str());
		conf.windowSize.y = atoi(dim[1].c_str());
	}

	if (cmd.check("--fullscreen") && cmd.check("--window")) {
		Log::err(argv[0], "--fullscreen and --window mutually exclusive");
		return false;
	}

	if (cmd.check("--fullscreen"))
		conf.fullscreen = true;

	if (cmd.check("--window"))
		conf.fullscreen = false;

	return true;
}

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

	// Init various libraries we use.
	libraries libs;

	if (!parseConfig(CLIENT_CONF_FILE))
		return 1;
	if (!parseCommandLine(argc, argv))
		return 1;
	if (conf.logLevel)
		Log::setMode(conf.logLevel);

	GameWindow window;
	if (!window.init(argv[0]))
		return 1;
	window.show();
	return 0;
}

