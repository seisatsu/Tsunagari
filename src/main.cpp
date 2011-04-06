/******************************
** Tsunagari Tile Engine     **
** main.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <fstream>
#include <istream>

#include "client.h"
#include "window.h"

bool parseClientConfig(ClientValues* conf)
{
	Json::Value root;
	Json::Value windowsize;
	Json::Reader reader;
	bool parsingSuccessful;

	// This probably won't be changed ever except maybe with a command line
	// option.
	std::ifstream file("./client.conf");

	// Here we load in the client config file. It's a little messy.
	parsingSuccessful = reader.parse(file, root); // Actual parsing.
	if (!parsingSuccessful) {
		printf("Client config failed to parse as JSON\n");
		return false;
	}

	// Begin loading in configuration values.
	conf->world = root.get("world", "_NONE_").asString();
	if (conf->world.compare("_NONE_") == 0) {
		printf("Client config didn't contain world value\n");
		return false;
	}

	conf->fullscreen = root.get("fullscreen", false).asBool();

	windowsize = root["windowsize"];
	if (windowsize.size() != 2) {
		printf("Client config windowsize didn't contain 2 values\n");
		return false;
	}

	conf->windowsize.x = windowsize[uint(0)].asUInt();
	conf->windowsize.y = windowsize[1].asUInt();

	return true;
}

/**
 * Engine entrypoint.
 *
 * The entire engine is encompassed in the GameWindow class.
 */
int main()
{
	ClientValues conf;

	if (!parseClientConfig(&conf))
		return 1; // Failed to load client config

	GameWindow window(conf.windowsize.x, conf.windowsize.y,
	        conf.fullscreen);
	window.show();
	window.initEntryWorld(conf.world);

	return 0;
}

