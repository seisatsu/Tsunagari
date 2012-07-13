/****************************
** Tsunagari Tile Engine   **
** bitrecord.cpp           **
** Copyright 2012 OmegaSDG **
****************************/

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

