/******************************
** Tsunagari Tile Engine     **
** main.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <ostream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <string.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <popt.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "window.h"

#ifndef LIBXML_TREE_ENABLED
#	error Tree must be enabled in libxml2
#endif

char* customConf;
char* verbosity;
int cache_ttl = 300;
char* size;

struct poptOption optionsTable[] = {
	{"conf", 'c', POPT_ARG_STRING, &customConf, 'c', "Client config file to use", "CONFFILE"},
	{"verbosity", 'v', POPT_ARG_STRING, &verbosity, 'v', "Log message level", "ERROR,DEVEL,DEBUG"},
	{"cache-ttl", 't', POPT_ARG_INT, &verbosity, 't', "Resource cache time-to-live", "SECONDS"},
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
	tern cache_enabled;
	tern cache_ttl;
	message_mode_t loglevel;
};

static void xmlErrorCb(void*, const char* msg, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, msg);
	sprintf(buf, msg, va_arg(ap, char*));
	Log::err(CLIENT_CONF_FILE, buf);
	va_end(ap);
}

/**
 * Load the values we need to start initializing the game from an XML file.
 *
 * We need to know what size window to create and which World to load. This
 * information will be stored in a XML file which we parse here.
 *
 * @param filename Name of the XML-encoded file to load from.
 *
 * @return ClientValues object if successful
 */
static ClientValues* parseConfig(const char* filename)
{
	xmlDoc* doc = NULL;
	xmlNode* root = NULL;

	doc = xmlReadFile(filename, NULL, XML_PARSE_NOBLANKS);

	if (!doc) {
		Log::err(filename, "Could not parse file");
		return NULL;
	}

	boost::shared_ptr<void> alwaysFreeTheDoc(doc, xmlFreeDoc);

	xmlValidCtxt ctxt;
	ctxt.error = xmlErrorCb;
	if (!xmlValidateDocument(&ctxt, doc)) {
		Log::err(filename, "XML document does not follow DTD");
		return NULL;
	}

	root = xmlDocGetRootElement(doc);
	xmlNode* node = root->xmlChildrenNode; // <client>
	xmlChar* str;

	/* Extract from XML object:
	 *  - name of World to load
	 *  - width, height, fullscreen-ness of Window
	 */
	ClientValues* conf = new ClientValues;
	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (!xmlStrncmp(node->name, BAD_CAST("world"), 6)) {
			xmlChar* str = xmlNodeGetContent(node);
			conf->world = (char*)str;
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("window"), 7)) {
			str = xmlGetProp(node, BAD_CAST("x"));
			conf->windowsize.x = atol((char*)str); // atol

			str = xmlGetProp(node, BAD_CAST("y"));
			conf->windowsize.y = atol((char*)str); // atol

			str = xmlGetProp(node, BAD_CAST("fullscreen"));
			conf->fullscreen = parseBool((char*)str);
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("cache"), 6)) {
			str = xmlGetProp(node, BAD_CAST("enabled"));
			conf->cache_enabled = (tern)parseBool((char*)str);
			str = xmlGetProp(node, BAD_CAST("ttl"));
			conf->cache_ttl = (tern)parseBool((char*)str);
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("logging"), 8)) {
			str = xmlGetProp(node, BAD_CAST("level"));
			if (!strcmp((char*)str, "error"))
				conf->loglevel = MM_SILENT;
			else if (!strcmp((char*)str, "devel"))
				conf->loglevel = MM_DEVELOPER;
			else if (!strcmp((char*)str, "debug"))
				conf->loglevel = MM_DEBUG;
		}
		node = node->next;
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
	initLibraries();

	ClientValues* conf = parseConfig(CLIENT_CONF_FILE);
	poptContext optCon = poptGetContext(NULL, argc, (const char**)argv, optionsTable, 0);
	poptSetOtherOptionHelp(optCon, "[WORLD FILE]");

	std::vector<std::string> dimensions;
	int c;
	while ((c = poptGetNextOpt(optCon)) >= 0) {
		switch (c) {
		case 'c':
			delete conf;
			conf = parseConfig(customConf);
			if (!conf)
				return 1;
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
			// FIXME: requires resource cache
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
	
	if (!conf)
		return 1;
	
	const char* customWorld = poptGetArg(optCon);
	if (customWorld)
		conf->world = customWorld;
	if (poptPeekArg(optCon))
		usage(optCon, "Specify a single world: e.g. babysfirst.world\n");
	poptFreeContext(optCon);

	if (conf->loglevel)
		Log::setMode(conf->loglevel);

	GameWindow window((unsigned)conf->windowsize.x,
	                  (unsigned)conf->windowsize.y,
	                  conf->fullscreen);
	if (window.init(conf->world))
		window.show();

	delete conf;

	cleanupLibraries();

	return 0;
}

