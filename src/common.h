/******************************
** Tsunagari Tile Engine     **
** common.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>

#include "log.h" // for message_mode_t

//! Game Movement Mode
enum movement_mode_t {
	TURN,
	TILE,
	NOTILE
};

struct ivec3 {
	int x, y, z;
};

//! Coordinate Type
typedef ivec3 icoord_t;

//! coord_t constructor
icoord_t icoord(int x, int y, int z);

//! 3D Cube Type
struct icube_t {
	int x1, x2;
	int y1, y2;
	int z1, z2;
};

//! cube_t constructor
icube_t icube(int x1, int y1, int z1,
              int x2, int y2, int z2);

//! Client Configuration
struct ClientValues {
	std::string world;
	movement_mode_t movemode;
	icoord_t windowsize;
	bool fullscreen;
	bool cache_enabled;
	int cache_ttl;
	int cache_size;
	message_mode_t loglevel;
};

//! Returns a bool from a "true"/"false" string.
bool parseBool(const std::string& s);

//! Split a string by a delimiter.
std::vector<std::string> splitStr(std::string str,
	const std::string& delimiter);

//! Convert an integer to a representative string.
std::string itostr(int in);

#endif

