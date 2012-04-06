/****************************
** Tsunagari Tile Engine   **
** python_random.h         **
** Copyright 2012 OmegaSDG **
****************************/

#ifndef PYTHON_RANDOM_H
#define PYTHON_RANDOM_H

#include <stdlib.h>

int pythonRandInt(int min, int max);
double pythonRandFloat(double min, double max);
void exportRandom();

#endif

