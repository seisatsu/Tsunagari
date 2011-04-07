/******************************
** Tsunagari Tile Engine     **
** main.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <iostream>
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
static bool parseClientConfig(const char* filename, ClientValues* conf)
{
	Json::Reader reader;
	Json::Value root, windowsize;

	std::ifstream file(filename);

	if (!reader.parse(file, root))
		return false;

	/* GET:
	 *  - name of World to load
	 *  - width, height, fullscreen-ness of Window
	 */
	conf->world = root.get("world", "_NONE_").asString();
	if (conf->world == "_NONE_") {
		std::cerr << "Error: " << CLIENT_CONF_FILE << ": \"world\" required.\n";
		return false;
	}

	windowsize = root["windowsize"];
	if (windowsize.size() != 2) {
		std::cerr << "Error: " << CLIENT_CONF_FILE << ": \"windowsize\" [2] required.\n";
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
	int masterReturnValue;

	if (!parseClientConfig(CLIENT_CONF_FILE, &conf)) {
		std::cerr << "Error: " << CLIENT_CONF_FILE << "\n";
		return 1;
	}

	GameWindow window(conf.windowsize.x, conf.windowsize.y,
		conf.fullscreen);

	masterReturnValue = window.initEntryWorld(conf.world);
	if (masterReturnValue != 0) {
		switch (masterReturnValue) {
			case 2:
				std::cerr << "Error: Entry point: World descriptor\n";
				break;
			case 3:
				std::cerr << "Error: Entry point: Sprite descriptor\n";
				break;
		}
		return masterReturnValue;
	}
	
	window.show();

	return 0;
}

