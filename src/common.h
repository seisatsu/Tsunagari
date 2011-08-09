/******************************
** Tsunagari Tile Engine     **
** common.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>

#include <libxml/tree.h>

#include "log.h"

//! Ternary Data Type
enum tern {
	T_True = true,
	T_False = false,
	T_None
};

//! Game Movement Mode
enum movement_mode_t {
	TURN,
	TILE,
	NOTILE
};

//! Coordinate Type
struct coord_t {
	long x;
	long y;
	long z;
};

//! coord_t constructor
coord_t coord(long x, long y, long z);

//! 3D Cube Type
struct cube_t {
	long x1, x2;
	long y1, y2;
	long z1, z2;
};

//! cube_t constructor
cube_t cube(long x1, long y1, long z1,
            long x2, long y2, long z2);

//! Client Configuration
struct ClientValues {
	std::string world;
	movement_mode_t movemode;
	coord_t windowsize;
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
std::string itostr(long in);

//! Get XML attribute (memory-safe).
std::string readXmlAttribute(xmlNode* node, const std::string attr);

//! Get XML element content (memory-safe).
std::string readXmlElement(xmlNode* node);

#endif

