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

//! Integer vector.
struct ivec3 {
	int x, y, z;
};

//! Real vector.
struct rvec3 {
	double x, y, z;
};

//! Coordinates.
typedef ivec3 icoord_t;
typedef rvec3 rcoord_t;

//! coord_t constructors.
icoord_t icoord(int x, int y, int z);
rcoord_t rcoord(double x, double y, double z);

//! 3D cube type.
struct icube_t {
	int x1, x2;
	int y1, y2;
	int z1, z2;
};

//! cube_t constructor.
icube_t icube(int x1, int y1, int z1,
              int x2, int y2, int z2);

//! Engine-wide user-confurable values.
struct ClientValues {
	std::string world;
	movement_mode_t moveMode;
	icoord_t windowSize;
	bool fullscreen;
	bool cacheEnabled;
	int cacheTTL;
	int cacheSize;
	message_mode_t logLevel;
};

//! Returns a bool from a "true"/"false" string.
bool parseBool(const std::string& s);

//! Split a string by a delimiter.
std::vector<std::string> splitStr(std::string str,
	const std::string& delimiter);

//! Convert an integer to a representative string.
std::string itostr(int in);

//! Returns true if the string contains only digits, whitespace, and minus.
bool isInteger(const std::string& s);

#endif

