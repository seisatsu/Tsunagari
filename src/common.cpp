/******************************
** Tsunagari Tile Engine     **
** common.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <string>
#include <string.h>
#include "common.h"

char* strtochar(std::string str) {
	char* out = new char[str.size()+1];
	out[str.size()] = 0;
	memcpy(out, str.c_str(), str.size());
	return out;
}

