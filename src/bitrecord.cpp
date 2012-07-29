/****************************
** Tsunagari Tile Engine   **
** bitrecord.cpp           **
** Copyright 2012 OmegaSDG **
****************************/

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

#include <Gosu/Input.hpp>

#include "bitrecord.h"
#include "window.h"

BitRecord BitRecord::fromGosuInput()
{
	size_t cnt = Gosu::numButtons;
	Gosu::Input& in = GameWindow::instance().input();

	BitRecord rec(cnt);
	for (size_t i = 0; i < cnt; i++)
		rec.states[i] = in.down(Gosu::Button((unsigned)i));

	return rec;
}

bool BitRecord::operator[] (size_t idx)
{
	return states[idx];
}

std::vector<size_t> BitRecord::diff(const BitRecord& other)
{
	std::vector<size_t> changes;

	for (size_t i = 0; i < states.size(); i++)
		if (states[i] != other.states[i])
			changes.push_back(i);
	return changes;
}

BitRecord::BitRecord(size_t length)
	: states(length)
{
}

