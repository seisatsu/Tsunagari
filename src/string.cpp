/*********************************
** Tsunagari Tile Engine        **
** string.cpp                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

// "OmegaSDG" is defined as Michael D. Reiley and Paul Merrill.

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to 
// deal in the Software without restriction, including without limitation the 
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
// IN THE SOFTWARE.
// **********

#include <ctype.h>
#include <sstream>
#include <stdlib.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp> // for iequals
#include <Gosu/Math.hpp>

#include "log.h"
#include "string.h"

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

/**
 * Matches "5-7,2,12-14" no whitespace.
 */
bool isRanges(const std::string& s)
{
	const int sign = 0;
	const int digit = 1;
	const int dash = 3;
	const int comma = 4;

	bool dashed = false;

	int state = sign;

	for (size_t i = 0; i < s.size(); i++) {
		char c = s[i];
		switch (state) {
		case sign:
			state++;
			if (c == '-' || c == '+') break;
		case digit:
			if (isdigit(c)) break;
			state++;
		case dash:
			state++;
			if (c == '-') {
				if (dashed) return false;
				dashed = true;
				state = sign;
				break;
			}
		case comma:
			state++;
			if (c == ',') {
				dashed = false;
				state = sign;
				break;
			}
			return false;
		}
	}
	return true;
}

bool parseBool(const std::string& s)
{
	// boost::equals is case-insensative
	return boost::iequals(s, "true") ||
	       boost::iequals(s, "yes") ||
	       boost::iequals(s, "on") ||
	    s == "1";
}

int parseUInt(const std::string& s)
{
	int i = atoi(s.c_str());
	return Gosu::clamp(i, 0, INT_MAX);
}

int parseInt100(const std::string& s)
{
	int i = atoi(s.c_str());
	return Gosu::clamp(i, 0, 100);
}

std::vector<std::string> splitStr(const std::string& input,
		const std::string& delimiter)
{
	std::vector<std::string> strlist;
	size_t i = 0;

	for (size_t pos = input.find(delimiter); pos != std::string::npos; pos = input.find(delimiter, i)) {
		if (input.size() != i) // Don't save empty strings
			strlist.push_back(input.substr(i, pos - i)); // Save
		i = pos + delimiter.size();
	}

	if (input.size() != i)
		strlist.push_back(input.substr(i));
	return strlist;
}

std::vector<int> parseRanges(const std::string& format)
{
	std::vector<int> ints;
	std::vector<std::string> ranges = splitStr(format, ",");
	BOOST_FOREACH(const std::string& range, ranges) {
		size_t dash = range.find("-");
		if (dash == std::string::npos) {
			if (!isInteger(range)) {
				Log::err("parseRanges", "not an integer");
				continue;
			}
			int i = atoi(range.c_str());
			ints.push_back(i);
		}
		else {
			std::string rngbeg = range.substr(0, dash);
			std::string rngend = range.substr(dash + 1);
			if (!isInteger(rngbeg) || !isInteger(rngend)) {
				Log::err("parseRanges", "not an integer");
				continue;
			}
			int beg = atoi(rngbeg.c_str());
			int end = atoi(rngend.c_str());
			if (beg > end) {
				Log::err("parseRanges", "beg > end");
				continue;
			}
			for (int i = beg; i <= end; i++)
				ints.push_back(i);
		}
	}
	return ints;
}

std::string itostr(int in)
{
	std::stringstream out;
	out << in;
	return out.str();
}

