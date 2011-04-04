/******************************
** Tsunagari Tile Engine     **
** common.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <stdint.h>

struct coord_t {
	uint32_t x;
	uint32_t y;
};

char* strtochar(std::string str);

#endif

