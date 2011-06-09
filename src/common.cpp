/******************************
** Tsunagari Tile Engine     **
** common.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <sstream>

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
	return s == "true" || s == "1";
}

std::vector<std::string> splitStr(std::string str, const std::string& delimiter)
{
	std::vector<std::string> strlist;
	size_t pos;
	
	pos = str.find(delimiter);
	while (pos != std::string::npos) {
		if (pos != std::string::npos || pos+1 != str.size()) {
			if (str.size() != 0) // Don't save empty strings
				strlist.push_back(str.substr(0, pos)); // Save
			str = str.substr(pos+delimiter.size()); // Cut delimiter
		}
		pos = str.find(delimiter);
	}
	return strlist;
}

std::string itostr(int32_t in)
{
	std::stringstream out;
	out << in;
	return out.str();
}

