/******************************
** Tsunagari Tile Engine     **
** main.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <fstream>
#include <istream>
#include <stdio.h>
#include <string>

#include <json/json.h>

#include "common.h"
#include "window.h"


/**
 * This probably won't be changed ever except maybe with a command line option.
 */
#define CLIENT_CONF_FILE "./client.conf"

/**
 * Values needed prior to creating the GameWindow.
 */
struct ClientValues {
	std::string world;
	coord_t windowsize;
	bool fullscreen;
};


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
static bool parseClientConfig(const std::string filename, ClientValues* conf)
{
	Json::Reader reader;
	Json::Value root, windowsize;

	std::ifstream file(filename);

	if (!reader.parse(file, root)) {
		printf("Client config failed to parse as JSON\n");
		return false;
	}

	/* GET:
	 *  - name of World to load
	 *  - width, height, fullscreen-ness of Window
	 */
	conf->world = root.get("world", "_NONE_").asString();
	if (conf->world == "_NONE_") {
		printf("Client config didn't contain world name\n");
		return false;
	}

	windowsize = root["windowsize"];
	if (windowsize.size() != 2) {
		printf("Client config windowsize didn't contain 2 values\n");
		return false;
	}

	conf->windowsize.x = windowsize[uint(0)].asUInt();
	conf->windowsize.y = windowsize[1].asUInt();

	conf->fullscreen = root.get("fullscreen", false).asBool();

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

	if (!parseClientConfig(CLIENT_CONF_FILE, &conf))
		return 1;

	GameWindow window(conf.windowsize.x, conf.windowsize.y,
	        conf.fullscreen);
	window.show();

	return 0;
}

