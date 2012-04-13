/*********************************
** Tsunagari Tile Engine        **
** common.h                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef COMMON_H
#define COMMON_H

#include "log.h" // for message_mode_t
#include "vec.h" // for icoord

//! Game Movement Mode
enum movement_mode_t {
	TURN,
	TILE,
	NOTILE
};

//! Engine-wide user-confurable values.
struct Conf {
	std::string worldFilename;
	verbosity_t verbosity;
	movement_mode_t moveMode;
	icoord windowSize;
	bool fullscreen;
	bool audioEnabled;
	bool cacheEnabled;
	int cacheTTL;
	int cacheSize;
	int persistInit;
	int persistCons;
};
extern Conf conf;

template<class T>
T wrap(T min, T value, T max)
{
	while (value < min)
		value += max;
	return value % max;
}

#endif

