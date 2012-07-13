/****************************
** Tsunagari Tile Engine   **
** bitrecord.h             **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef BITRECORD_H
#define BITRECORD_H

#include <vector>

class BitRecord {
public:
	static BitRecord fromGosuInput();

	bool operator[] (size_t idx);
	std::vector<size_t> diff(const BitRecord& other);

private:
	BitRecord(size_t length);

	std::vector<bool> states;
};

#endif

