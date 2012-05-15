/****************************
** Tsunagari Tile Engine   **
** random.h                **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>

	//! Produce a random integer.
	/*!
		@param min Minimum value.
		@param max Maximum value.
		@return random integer between min and max.
	*/
int randInt(int min, int max);

	//! Produce a random double floating point number.
	/*!
		@param min Minimum value.
		@param max Maximum value.
		@return random floating point number between min and max.
	*/
double randFloat(double min, double max);

void exportRandom();

#endif

