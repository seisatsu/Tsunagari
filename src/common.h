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

struct coord_t {
	uint32_t x;
	uint32_t y;
	uint32_t z;
};

//! coord_t constructor
coord_t coord(uint32_t x, uint32_t y, uint32_t z);

bool parseBool(const std::string& s);

//! Split a string by a delimiter.
std::vector<std::string> splitStr(std::string str, std::string delimiter);

#endif

