/******************************
** Tsunagari Tile Engine     **
** common.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include "common.h"

coord_t coord(uint32_t x, uint32_t y, uint32_t z)
{
	coord_t c;
	c.x = x;
	c.y = y;
	c.z = z;
	return c;
}

bool parseBool(const std::string& s)
{
	return s == "true";
}

std::vector<std::string> splitStr(std::string str, std::string delimiter)
{
	std::vector<std::string> strlist;
	size_t pos;
	bool run = true;
	
	while (run) {
		pos = str.find(delimiter);
		
		if (pos==std::string::npos)
			run = false;
		
		if (pos!=std::string::npos || pos+1 != str.size()) {
			if (pos!=0) // Don't save empty strings
				strlist.push_back(str.substr(0, pos)); // Save
			str = str.substr(pos+delimiter.size()); // Cut delimiter
		}
	}
	return strlist;
}

