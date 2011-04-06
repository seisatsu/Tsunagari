/******************************
** Tsunagari Tile Engine     **
** main.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include <fstream>
#include <istream>

#include "client.h"
#include "window.h"

/**
 * Engine entrypoint.
 *
 * The entire engine is encompassed in the GameWindow class.
 */

bool parseClientConfig() {
	Json::Value root;
	Json::Value windowsize;
	Json::Reader reader;
	bool parsingSuccessful;
	
	__cconf = new ClientValues;
	
	std::ifstream file("./client.conf");
	
	// Here we load in the client config file. It's a little messy.
	parsingSuccessful = reader.parse(file, root); // Actual parsing.
	if (!parsingSuccessful)
		return false;
	
	// Begin loading in configuration values.
	__cconf->world = root.get("world", "_NONE_").asString(); // world
	if (__cconf->world.compare("_NONE_") == 0)
		return false;
	
	__cconf->fullscreen = root.get("name", false).asBool(); // fullscreen
	
	windowsize = root["windowsize"]; // windowsize
	if (windowsize.size() != 2)
		return false;
	__cconf->windowsize->x = windowsize[uint(0)].asUInt();
	__cconf->windowsize->y = windowsize[1].asUInt();
	
	return true;
}

int main()
{
	if (!parseClientConfig())
		return 1; // Failed to load client config
	
	GameWindow window(__cconf->windowsize->x, __cconf->windowsize->y, __cconf->fullscreen);
	window.show();
	
	delete __cconf;
	
	return 0;
}

