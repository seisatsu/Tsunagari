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
#include <popt.h>
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

char* customConf;
char* verbosity;
int cache_ttl;
char* size;

struct poptOption optionsTable[] = {
	{"conf", 'c', POPT_ARG_STRING, &customConf, 'c', "Client config file to use", "CONFFILE"},
	{"verbosity", 'v', POPT_ARG_STRING, &verbosity, 'v', "Log message level", "ERROR,DEVEL,DEBUG"},
	{"cache-ttl", 't', POPT_ARG_INT, &cache_ttl, 't', "Resource cache time-to-live", "SECONDS"},
	{"size", 's', POPT_ARG_STRING, &size, 's', "Window dimensions", "WxH"},
	{"fullscreen", 'f', POPT_ARG_NONE, 0, 'f', "Run in fullscreen mode", NULL},
	{"window", 'w', POPT_ARG_NONE, 0, 'w', "Run in windowed mode", NULL},
	{"query", 'q', POPT_ARG_NONE, 0, 'q', "Query compiled-in engine defaults", NULL},
	POPT_AUTOHELP
	{NULL, 0, 0, NULL, 0}
};

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
	ClientValues* conf = new ClientValues;
	
	std::ifstream config(filename);
	if (!config) {
		Log::err(CLIENT_CONF_FILE, "could not parse config");
		delete conf;
		return NULL;
	}
	
	std::set<std::string> options;
	std::map<std::string, std::string> parameters;
	options.insert("*");

	namespace pod = boost::program_options::detail;

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

static void usage(poptContext optCon, const char* msg)
{
	poptPrintUsage(optCon, stderr, 0);
	std::cerr << msg;
	exit(1);
}

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
	
	if (conf && conf->loglevel)
		Log::setMode(conf->loglevel);
	
	poptContext optCon = poptGetContext(NULL, argc, (const char**)argv, optionsTable, 0);
	poptSetOtherOptionHelp(optCon, "[WORLD FILE]");
	
	std::vector<std::string> dimensions;
	int c;
	while ((c = poptGetNextOpt(optCon)) >= 0) {
		switch (c) {
		case 'c':
			delete conf;
			conf = parseConfig(customConf);
			if (conf && conf->loglevel)
				Log::setMode(conf->loglevel);
			if (!conf) {
				Log::err(customConf, "loading config failed");
				delete conf;
				return 1;
			}
			break;
		case 'v':
			if (!strcmp(verbosity, "error"))
				conf->loglevel = MM_SILENT;
			else if (!strcmp(verbosity, "devel"))
				conf->loglevel = MM_DEVELOPER;
			else if (!strcmp(verbosity, "debug"))
				conf->loglevel = MM_DEBUG;
			else
				usage(optCon, "Log level must be one of (error, devel, debug)\n");
			break;
		case 't':
			if (cache_ttl == 0)
				conf->cache_enabled = false;
			conf->cache_ttl = cache_ttl;
			break;
		case 's':
			dimensions = splitStr(size, "x");
			if (dimensions.size() != 2)
				usage(optCon, "Dimensions must be in form WxH: e.g. 800x600\n");
			conf->windowsize.x = atoi(dimensions[0].c_str());
			conf->windowsize.y = atoi(dimensions[1].c_str());
			break;
		case 'f':
			conf->fullscreen = true;
			break;
		case 'w':
			conf->fullscreen = false;
			break;
		case 'q':
			defaultsQuery();
			return 0;
			break;
		}
	}

	if (!conf) {
		Log::err(CLIENT_CONF_FILE, "loading config failed");
		delete conf;
		return 1;
	}
	
	const char* customWorld = poptGetArg(optCon);
	if (customWorld)
		conf->world = customWorld;
	if (poptPeekArg(optCon))
		usage(optCon, "Specify a single world: e.g. babysfirst.world\n");
	poptFreeContext(optCon);
	
	GameWindow window((unsigned)conf->windowsize.x,
	                  (unsigned)conf->windowsize.y,
	                  conf->fullscreen);
	if (window.init(conf->world))
		window.show();
	
	delete conf;
	
	cleanupLibraries();
	
	return 0;
}

