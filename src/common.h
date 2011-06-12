/******************************
** Tsunagari Tile Engine     **
** common.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <string>
#include <vector>

//! Ternary Data Type
enum tern {
	T_True = true,
	T_False = false,
	T_None
};

struct coord_t {
	int32_t x;
	int32_t y;
	int32_t z;
};

//! coord_t constructor
coord_t coord(int32_t x, int32_t y, int32_t z);

bool parseBool(const std::string& s);

//! Split a string by a delimiter.
std::vector<std::string> splitStr(std::string str, const std::string& delimiter);

std::string itostr(int32_t in);

#endif

