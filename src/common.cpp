/******************************
** Tsunagari Tile Engine     **
** common.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <ctype.h>
#include <sstream>

#include <boost/algorithm/string.hpp> // for iequals

#include "common.h"


icube_t icube(int x1, int y1, int z1,
              int x2, int y2, int z2)
{
	icube_t c;
	c.x1 = x1;
	c.y1 = y1;
	c.z1 = z1;
	c.x2 = x2;
	c.y2 = y2;
	c.z2 = z2;
	return c;
}

bool parseBool(const std::string& s)
{
	return boost::iequals(s, "true") ||
	       boost::iequals(s, "yes") ||
	       boost::iequals(s, "on") ||
	    s == "1";
}

std::vector<std::string> splitStr(std::string str, const std::string& delimiter)
{
	std::vector<std::string> strlist;
	size_t pos;

	pos = str.find(delimiter);

	while (pos != std::string::npos) {
		if (pos != std::string::npos || pos+1 != str.size()) {
			if (str.size() && pos) // Don't save empty strings
				strlist.push_back(str.substr(0, pos)); // Save
			str = str.substr(pos+delimiter.size()); // Cut delimiter
		}
		pos = str.find(delimiter);
	}

	if (pos == std::string::npos && str.size() != 0)
		strlist.push_back(str);

	return strlist;
}

std::string itostr(int in)
{
	std::stringstream out;
	out << in;
	return out.str();
}

// FIXME: "1 2", " ", and "" are considered valid, " -3" not valid
bool isInteger(const std::string& s)
{
	for (unsigned i = 0; i < s.size(); i++) {
		char c = s[i];
		if (isdigit(c) || isspace(c) || (c == '-' && i == 0))
			continue;
		return false;
	}
	return true;
}

// FIXME: "1 2", " ", and "" are considered valid, " -3" not valid
bool isDecimal(const std::string& s)
{
	bool seenDot = false;
	for (unsigned i = 0; i < s.size(); i++) {
		char c = s[i];
		if (c == '.' && !seenDot) {
			seenDot = true;
			continue;
		}
		if (isdigit(c) || isspace(c) || (c == '-' && i == 0))
			continue;
		return false;
	}
	return true;
}

