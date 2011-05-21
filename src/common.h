/******************************
** Tsunagari Tile Engine     **
** common.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <string>

struct coord_t {
	uint32_t x;
	uint32_t y;
	uint32_t z;
};

//! coord_t constructor
coord_t coord(uint32_t x, uint32_t y, uint32_t z);

bool parseBool(const std::string& s);

#endif

