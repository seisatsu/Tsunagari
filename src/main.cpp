/******************************
** Tsunagari Tile Engine     **
** main.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <stdarg.h>
#include <string>

#include <boost/shared_ptr.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "window.h"

#ifndef LIBXML_TREE_ENABLED
#	error Tree must be enabled in libxml2
#endif

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
} *CLIENT_CONFIG;

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
 * Load the values we need to start initializing the game from a JSON file.
 *
 * We need to know what size window to create and which World to load. This
 * information will be stored in a JSON file which we parse here.
 *
 * @param filename Name of the JSON-encoded file to load from.
 * @param conf Values are stored here.
 *
 * @return True if successful
 */
static bool parseClientConfig(const char* filename, ClientValues* conf)
{
	xmlDoc* doc = NULL;
	xmlNode* root = NULL;

	doc = xmlReadFile(filename, NULL, XML_PARSE_NOBLANKS);

	if (!doc) {
		Log::err(filename, "Could not parse file");
		return false;
	}

	boost::shared_ptr<void> alwaysFreeTheDoc(doc, xmlFreeDoc);

	xmlValidCtxt ctxt;
	ctxt.error = xmlErrorCb;
	if (!xmlValidateDocument(&ctxt, doc)) {
		Log::err(filename, "XML document does not follow DTD");
		return false;
	}

	root = xmlDocGetRootElement(doc);
	xmlNode* node = root->xmlChildrenNode; // <client>
	xmlChar* str;

	/* Extract from XML object:
	 *  - name of World to load
	 *  - width, height, fullscreen-ness of Window
	 */
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
			if (str == NULL)
				conf->cache_enabled = T_None;
			else
				conf->cache_enabled = (tern)parseBool((char*)str);
			str = xmlGetProp(node, BAD_CAST("ttl"));
			if (str == NULL)
				conf->cache_ttl = T_None;
			else
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
			else {
				Log::err(filename, "Invalid logging level defined");
				return false;
			}
		}
		node = node->next;
	}

	return true;
}

/**
 * Load client config and instantiate window.
 *
 * The client config tells us our window parameters along with which World
 * we're going to load. The GameWindow class then loads and plays the game.
 */
int main()
{
	CLIENT_CONFIG = new ClientValues;

	/*
	 * This initializes the library and checks for potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	LIBXML_TEST_VERSION

	if (!parseClientConfig(CLIENT_CONF_FILE, CLIENT_CONFIG))
		return 1;

	if (CLIENT_CONFIG->loglevel)
		Log::setMode(CLIENT_CONFIG->loglevel);

	GameWindow window((unsigned)CLIENT_CONFIG->windowsize.x,
	                  (unsigned)CLIENT_CONFIG->windowsize.y,
	                  CLIENT_CONFIG->fullscreen);
	if (!window.init(CLIENT_CONFIG->world))
		return 1;
	window.show();

	// Clean the XML library.
	xmlCleanupParser();

	return 0;
}

