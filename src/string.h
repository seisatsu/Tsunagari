/*********************************
** Tsunagari Tile Engine        **
** string.h                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef STRING_H
#define STRING_H

#include <string>
#include <vector>

//! Return a bool from a "true"/"false" string.
bool parseBool(const std::string& s);

//! Split a string by a delimiter.
std::vector<std::string> splitStr(std::string str,
	const std::string& delimiter);

//! Convert an integer to a representative string.
std::string itostr(int in);

//! Returns true if the string contains only digits, whitespace, and minus.
bool isInteger(const std::string& s);

//! Returns true if the string contains only digits, whitespace, minus, and
//! period.
bool isDecimal(const std::string& s);

#endif

