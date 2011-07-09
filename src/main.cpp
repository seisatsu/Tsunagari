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

#include <boost/config.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "window.h"

#ifdef _WINDOWS
	#include <Windows.h>
#endif

#ifndef LIBXML_TREE_ENABLED
	#error Tree must be enabled in libxml2
#endif

/**
 * Values needed prior to creating the GameWindow.
 */
struct ClientValues {
	std::string world;
	coord_t windowsize;
	bool fullscreen;
	bool cache_enabled;
	unsigned int cache_ttl;
	message_mode_t loglevel;
};

/* Output compiled-in engine defaults. */
static void defaultsQuery()
{
	std::cerr << "CLIENT_CONF_FILE:                       " 
		<< CLIENT_CONF_FILE << std::endl;
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
	
	if (parameters["cache.enable"].empty())
		conf->cache_enabled = true;
	else if (parseBool(parameters["cache.enable"]))
		conf->cache_enabled = true;
	else
		conf->cache_enabled = false;
	
	if (parameters["cache.ttl"].empty())
		conf->cache_ttl = CACHE_EMPTY_TTL;
	else {
		if (atoi(parameters["cache.ttl"].c_str()) == 0)
			conf->cache_enabled = 0;
		conf->cache_ttl = atoi(parameters["cache.ttl"].c_str());
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
	namespace po = boost::program_options;
	
	po::options_description desc("Available options");
	desc.add_options()
		("help,h", "Show this help message")
		("gameworld,g", po::value<std::string>(), "Game world to play")
		("config,c", po::value<std::string>(), "Client config file to use")
		("verbosity,v", po::value<std::string>(), "Log message level (error,devel,debug)")
		("cache-ttl,t", po::value<unsigned int>(), "Resource cache time-to-live")
		("cache-size,m", po::value<unsigned int>(), "Resource cache size in megabytes")
		("size,s", po::value<std::string>(), "Window dimensions (WxH)")
		("fullscreen,f", "Run in fullscreen mode")
		("window,w", "Run in windowed mode")
		("query,q", "Query compiled-in engine defaults")
	;
	
	po::positional_options_description p;
	p.add("gameworld", -1);
	
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
		options(desc).positional(p).run(), vm);
	po::notify(vm);
	
	if (vm.count("help")) {
		std::cout << desc << "\n";
		return false;
	}
	
	if (vm.count("config")) {
		delete conf;
		conf = parseConfig(vm["config"].as<std::string>().c_str());
		if (!conf) {
			Log::err(vm["config"].as<std::string>(), "loading config failed");
			return false;
		}
	}
	
	if (vm.count("gameworld"))
		conf->world = vm["gameworld"].as<std::string>();
	
	if (vm.count("verbosity")) {
		if (!vm["verbosity"].as<std::string>().compare("error"))
			conf->loglevel = MM_SILENT;
		else if (!vm["verbosity"].as<std::string>().compare("devel"))
			conf->loglevel = MM_DEVELOPER;
		else if (!vm["verbosity"].as<std::string>().compare("debug"))
			conf->loglevel = MM_DEBUG;
		else {
			Log::err(argv[0], "invalid argument for --verbosity");
			return false;
		}
	}
	
	if (vm.count("cache-ttl")) {
		conf->cache_ttl = vm["cache-ttl"].as<unsigned int>();
		if (vm["cache-ttl"].as<unsigned int>() == 0)
			conf->cache_enabled = false;
	}
	
	if (vm.count("cache-size")) {
		//TODO: Merge cache branch.
	}
	
	if (vm.count("size")) {
		std::vector<std::string> dim = 
			splitStr(vm["size"].as<std::string>(), "x");
		if (dim.size() != 2) {
			Log::err(argv[0], "invalid argument for --size");
			return false;
		}
		conf->windowsize.x = atoi(dim[0].c_str());
		conf->windowsize.y = atoi(dim[1].c_str());
	}
	
	if (vm.count("fullscreen") && vm.count("window")) {
		Log::err(argv[0], "--fullscreen and --window mutually exclusive");
		return false;
	}
	
	if (vm.count("fullscreen"))
		conf->fullscreen = true;
	
	if (vm.count("window"))
		conf->fullscreen = false;
	
	if (vm.count("query")) {
		defaultsQuery();
		return false;
	}
	
	return true;
}

static void initLibraries()
{
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
	
	GameWindow window((unsigned)conf->windowsize.x,
	                  (unsigned)conf->windowsize.y,
	                  conf->fullscreen);

	if (window.init(conf->world))
		window.show();
	
	delete conf;
	
	cleanupLibraries();
	
	return 0;
}

