/*********************************
** Tsunagari Tile Engine        **
** common.h                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>

#include "log.h" // for message_mode_t
#include "vec.h" // for icoord

//! Game Movement Mode
enum movement_mode_t {
	TURN,
	TILE,
	NOTILE
};

/**
 * Virtual integer coordinate.
 *
 * x and y are the same as a physical integer coordinate.
 * z is a virtual layer depth within an Area.
 */
struct vicoord
{
	vicoord() {}
	vicoord(int x, int y, double z): x(x), y(y), z(z) {}

	int x, y;
	double z;
};

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
struct TsunagariConf {
	std::string worldFilename;
	message_mode_t logLevel;
	movement_mode_t moveMode;
	icoord windowSize;
	bool fullscreen;
	bool audioEnabled;
	bool cacheEnabled;
	int cacheTTL;
	int cacheSize;
};
extern TsunagariConf conf;

//! Return a bool from a "true"/"false" string.
bool parseBool(const std::string& s);

//! Split a string by a delimiter.
std::vector<std::string> splitStr(std::string str,
	const std::string& delimiter);

//! Convert an integer to a representative string.
std::string itostr(int in);

//! Returns true if the string contains only digits, whitespace, and minus.
bool isInteger(const std::string& s);

//! Returns true if the string contains only digits, whitespace, minus, and
//! period.
bool isDecimal(const std::string& s);

template<class T>
T wrap(T min, T value, T max)
{
	while (value < min)
		value += max;
	return value % max;
}

#endif

