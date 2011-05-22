/******************************
** Tsunagari Tile Engine     **
** main.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <stdarg.h>
#include <string>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "common.h"
#include "log.h"
#include "window.h"

#ifndef LIBXML_TREE_ENABLED
#	error Tree must be enabled in libxml2
#endif

/**
 * These probably won't be changed ever except maybe with a command line option.
 */
#define CLIENT_CONF_FILE "./client.conf"
#define MESSAGE_MODE MM_DEBUG

/**
 * Values needed prior to creating the GameWindow.
 */
struct ClientValues {
	std::string world;
	coord_t windowsize;
	bool fullscreen;
};

void xmlErrorCb(void*, const char* msg, ...)
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

	/*
	 * This initializes the library and checks for potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */
	LIBXML_TEST_VERSION

	doc = xmlReadFile(filename, NULL, XML_PARSE_NOBLANKS);

	if (!doc) {
		Log::err(filename, "Could not parse file");
		return false;
	}
	
	xmlValidCtxt ctxt;
	ctxt.error = xmlErrorCb;
	if (!xmlValidateDocument(&ctxt, doc)) {
		Log::err(filename, "XML document does not follow DTD");
		return false;
	}

	root = xmlDocGetRootElement(doc);
	xmlNode* node = root->xmlChildrenNode; // <client>
	
	/* Extract from XML object:
	 *  - name of World to load
	 *  - width, height, fullscreen-ness of Window
	 */
	node = node->xmlChildrenNode;
	while (node != NULL) {
		if (!xmlStrncmp(node->name, BAD_CAST("world"), 6)) {
			conf->world = (char*)xmlNodeListGetString(doc,
					node->xmlChildrenNode, 1);
		}
		else if (!xmlStrncmp(node->name, BAD_CAST("window"), 7)) {
			xmlChar* str;

			str = xmlGetProp(node, BAD_CAST("x"));
			conf->windowsize.x = atol((char*)str); // atol

			str = xmlGetProp(node, BAD_CAST("y"));
			conf->windowsize.y = atol((char*)str); // atol
				
			str = xmlGetProp(node, BAD_CAST("fullscreen"));
			conf->fullscreen = parseBool((char*)str);
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
	ClientValues conf;
	Log::setMode(MESSAGE_MODE);

	if (!parseClientConfig(CLIENT_CONF_FILE, &conf))
		return 1;

	GameWindow window(conf.windowsize.x, conf.windowsize.y,
		conf.fullscreen);
	if (!window.init(conf.world))
		return 1;
	window.show();

	return 0;
}

