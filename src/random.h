/****************************
** Tsunagari Tile Engine   **
** random.h                **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>

//! Produce a random integer between min and max.
int randInt(int min, int max);

//! Produce a random double between min and max.
double randFloat(double min, double max);

void exportRandom();

#endif

