/******************************
** Tsunagari Tile Engine     **
** main.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <fstream>
#include <istream>
#include <string>

#include <json/json.h>

#include "common.h"
#include "log.h"
#include "window.h"

/**
 * These probably won't be changed ever except maybe with a command line option.
 */
#define CLIENT_CONF_FILE "./client.conf"
#define MESSAGE_MODE MM_DEVELOPER

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

	if (!reader.parse(file, root)) {
		Log::err(CLIENT_CONF_FILE, "File missing.");
		return false;
	}

	/* Extract from JSON object:
	 *  - name of World to load
	 *  - width, height, fullscreen-ness of Window
	 */
	conf->world = root["world"].asString();
	if (conf->world.empty()) {
		Log::err(CLIENT_CONF_FILE, "\"world\" required.");
		return false;
	}

	windowsize = root["windowsize"];
	if (windowsize.size() != 2) {
		Log::err(CLIENT_CONF_FILE, "\"windowsize\" [2] required.");
		return false;
	}

	conf->windowsize.x = windowsize[0u].asUInt();
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

