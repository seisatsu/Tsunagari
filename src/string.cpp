/*********************************
** Tsunagari Tile Engine        **
** string.cpp                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <ctype.h>
#include <sstream>

#include <boost/algorithm/string.hpp> // for iequals

#include "string.h"

bool parseBool(const std::string& s)
{
	// boost::equals is case-insensative
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

