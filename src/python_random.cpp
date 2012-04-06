/****************************
** Tsunagari Tile Engine   **
** python_random.cpp       **
** Copyright 2012 OmegaSDG **
****************************/

#include "python.h"
#include "python_random.h"

int pythonRandInt(int min, int max)
{
	return rand() % (max-min) + min;
}

double pythonRandFloat(double min, double max)
{
        int i = rand();
        double d = (double)i / RAND_MAX;
        return d * (max - min) + min;
}

void exportRandom()
{
	using namespace boost::python;
	pythonAddFunction("randint", pythonRandInt);
	pythonAddFunction("randfloat", pythonRandFloat);
}

