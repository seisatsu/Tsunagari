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

/**
 * Matches regex /\s*-?\d+/
 */
bool isInteger(const std::string& s)
{
	const int space = 0;
	const int sign = 1;
	const int digit = 2;

	int state = space;

	for (size_t i = 0; i < s.size(); i++) {
		char c = s[i];
		if (state == space) {
		       if (isspace(c)) continue;
		       else state++;
		}
		if (state == sign) {
			state++;
			if (c == '-') continue;
		}
		if (state == digit) {
			if (isdigit(c)) continue;
			else return false;
		}
	}
	return true;
}

/**
 * Matches regex /\s*-?\d+\.?\d* /   [sic: star-slash ends comment]
 */
bool isDecimal(const std::string& s)
{
	const int space = 0;
	const int sign = 1;
	const int digit = 2;
	const int dot = 3;
	const int digit2 = 4;

	int state = space;

	for (size_t i = 0; i < s.size(); i++) {
		char c = s[i];
		switch (state) {
		case space:
		       if (isspace(c)) continue;
		       else state++;
		case sign:
			state++;
			if (c == '-') continue;
		case digit:
			if (isdigit(c)) continue;
			else state++;
		case dot:
			state++;
			if (c == '.') continue;
			else return false;
		case digit2:
			if (isdigit(c)) continue;
			else return false;
		}
	}
	return true;
}

