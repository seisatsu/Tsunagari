/***********************************
** Tsunagari Tile Engine          **
** formatter.cpp                  **
** Copyright 2013 PariahSoft LLC  **
***********************************/

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

#include "formatter.h"

Formatter::Formatter(std::string format)
	: result(format), pos(0)
{
	findNextPlaceholder();
}

Formatter::~Formatter()
{
}

Formatter::operator const std::string&()
{
	assert(pos == result.size());

	return result;
}

void Formatter::findNextPlaceholder()
{
	assert(pos <= result.size());

	size_t next = result.find("%", pos);
	if (next != std::string::npos)
		pos = next;
	else
		pos = result.size();
}


template<>
std::string Formatter::format(const int& data)
{
	char buf[512];
	sprintf(buf, "%i", data);
	return std::string(buf);
}

template<>
std::string Formatter::format(const double& data)
{
	char buf[512];
	sprintf(buf, "%f", data);
	return std::string(buf);
}

typedef char* cstring;
template<>
std::string Formatter::format(const cstring& data)
{
	return std::string(data);
}

template<>
std::string Formatter::format(const std::string& data)
{
	return data;
}