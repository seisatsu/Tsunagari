/******************************
** Tsunagari Tile Engine     **
** common.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef COMMON_H
#define COMMON_H

#include <string>

struct coord_t {
	unsigned long x;
	unsigned long y;
};

char* strtochar(std::string str);

#endif

