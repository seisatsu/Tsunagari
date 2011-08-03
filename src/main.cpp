/******************************
** Tsunagari Tile Engine     **
** main.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <time.h>

#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <libxml/parser.h>

#include "common.h"
#include "config.h"
#include "cmd.h"
#include "log.h"
#include "window.h"

#ifdef _WINDOWS
	#include <Windows.h>
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
 * @return ClientValues object if successful
 */
static ClientValues* parseConfig(const char* filename)
{
	namespace pod = boost::program_options::detail;

	ClientValues* conf = new ClientValues;

	conf->cache_enabled = CACHE_EMPTY_TTL && CACHE_MAX_SIZE;

	std::ifstream config(filename);
	if (!config) {
		Log::err(filename, "could not parse config");
		delete conf;
		return NULL;
	}

	std::set<std::string> options;
	std::map<std::string, std::string> parameters;
	options.insert("*");

	for (pod::config_file_iterator i(config, options), e ; i != e; ++i) {
		parameters[i->string_key] = i->value[0];
	}

	if (parameters["engine.world"].empty()) {
		Log::err(filename, "\"[engine] world\" option expected");
		return NULL;
	}
	else
		conf->world = parameters["engine.world"];

	if (parameters["window.width"].empty()) {
		Log::err(filename, "\"[window] width\" option expected");
		return NULL;
	}
	else
		conf->windowsize.x = atoi(parameters["window.width"].c_str());

	if (parameters["window.height"].empty()) {
		Log::err(filename, "\"[window] height\" option expected");
		return NULL;
	}
	else
		conf->windowsize.y = atoi(parameters["window.height"].c_str());

	if (parameters["window.fullscreen"].empty()) {
		Log::err(filename, "\"[window] fullscreen\" option expected");
		return NULL;
	}
	else
		conf->fullscreen = parseBool(parameters["window.fullscreen"]);

	if (!parameters["cache.enable"].empty()) {	
		if (parseBool(parameters["cache.enable"]))
			conf->cache_enabled = true;
		else
			conf->cache_enabled = false;
	}

	if (parameters["cache.ttl"].empty())
		conf->cache_ttl = CACHE_EMPTY_TTL;
	else {
		if (atoi(parameters["cache.ttl"].c_str()) == 0)
			conf->cache_enabled = 0;
		conf->cache_ttl = atoi(parameters["cache.ttl"].c_str());
	}

	if (parameters["cache.size"].empty())
		conf->cache_size = CACHE_MAX_SIZE;
	else {
		if (atoi(parameters["cache.size"].c_str()) == 0)
			conf->cache_enabled = 0;
		conf->cache_size = atoi(parameters["cache.size"].c_str());
	}

	if (parameters["engine.loglevel"].empty())
		conf->loglevel = MESSAGE_MODE;
	else if (parameters["engine.loglevel"] == "error" || 
	    parameters["engine.loglevel"] == "Error" || 
	    parameters["engine.loglevel"] == "ERROR")
		conf->loglevel = MM_SILENT;
	else if (parameters["engine.loglevel"] == "devel" || 
	    parameters["engine.loglevel"] == "Devel" || 
	    parameters["engine.loglevel"] == "DEVEL")
	conf->loglevel = MM_DEVELOPER;
	else if (parameters["engine.loglevel"] == "debug" || 
	    parameters["engine.loglevel"] == "Debug" || 
	    parameters["engine.loglevel"] == "DEBUG")
	conf->loglevel = MM_DEBUG;
	else {
		Log::err(filename, "unknown value for \"[engine] loglevel\", using default");
		conf->loglevel = MESSAGE_MODE;
	}

	return conf;
}

/* Parse and process command line options and arguments. */
static bool parseCommandLine(int argc, char* argv[], ClientValues* conf)
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
		delete conf;
		conf = parseConfig(cmd.get("--config").c_str());
		if (!conf) {
			Log::err(cmd.get("--config"), "loading config failed");
			return false;
		}
	}
	
	if (cmd.check("--gameworld"))
		conf->world = cmd.get("--gameworld");
	
	if (cmd.check("--verbosity")) {
		if (!cmd.get("--verbosity").compare("error"))
			conf->loglevel = MM_SILENT;
		else if (!cmd.get("--verbosity").compare("devel"))
			conf->loglevel = MM_DEVELOPER;
		else if (!cmd.get("--verbosity").compare("debug"))
			conf->loglevel = MM_DEBUG;
		else {
			Log::err(argv[0], "invalid argument for --verbosity");
			return false;
		}
	}
	
	if (cmd.check("--cache-ttl")) {
		conf->cache_ttl = atoi(cmd.get("--cache-ttl").c_str());
		if (conf->cache_ttl == 0)
			conf->cache_enabled = false;
	}
	
	if (cmd.check("--cache-size")) {
		conf->cache_size = atoi(cmd.get("--cache-size").c_str());
		if (conf->cache_size == 0)
			conf->cache_enabled = false;
	}
	
	if (cmd.check("--size")) {
		std::vector<std::string> dim = splitStr(cmd.get("--size"), "x");
		if (dim.size() != 2) {
			Log::err(argv[0], "invalid argument for --size");
			return false;
		}
		conf->windowsize.x = atoi(dim[0].c_str());
		conf->windowsize.y = atoi(dim[1].c_str());
	}
	
	if (cmd.check("--fullscreen") && cmd.check("--window")) {
		Log::err(argv[0], "--fullscreen and --window mutually exclusive");
		return false;
	}
	
	if (cmd.check("--fullscreen"))
		conf->fullscreen = true;
	
	if (cmd.check("--window"))
		conf->fullscreen = false;
	
	return true;
}

static void initLibraries()
{
	// Initialize the C library's random seed.
	srand((unsigned)time(NULL));

	/*
	 * This initializes the library and checks for potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	LIBXML_TEST_VERSION
}

static void cleanupLibraries()
{
	// Clean the XML library.
	xmlCleanupParser();
}

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
	
	initLibraries();
	
	ClientValues* conf = parseConfig(CLIENT_CONF_FILE);

	if (!parseCommandLine(argc, argv, conf)) {
		delete conf;
		return 1;
	}
	
	if (conf && conf->loglevel)
		Log::setMode(conf->loglevel);
	
	if (!conf) {
		Log::err(CLIENT_CONF_FILE, "loading config failed");
		delete conf;
		return 1;
	}
	
	GameWindow window(conf);

	if (window.init())
		window.show();
	
	delete conf;
	
	cleanupLibraries();
	
	return 0;
}

