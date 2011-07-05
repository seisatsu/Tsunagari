/******************************
** Tsunagari Tile Engine     **
** common.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>

//! Ternary Data Type
enum tern {
	T_True = true,
	T_False = false,
	T_None
};

//! Coordinate Type
struct coord_t {
	long x;
	long y;
	long z;
};

//! coord_t constructor
coord_t coord(long x, long y, long z);

//! Returns a bool from a "true"/"false" string.
bool parseBool(const std::string& s);

//! Split a string by a delimiter.
std::vector<std::string> splitStr(std::string str, const std::string& delimiter);

//! Convert an integer to a representative string.
std::string itostr(long in);

#endif

