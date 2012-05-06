/****************************
** Tsunagari Tile Engine   **
** random.cpp              **
** Copyright 2012 OmegaSDG **
****************************/

#include "python.h"
#include "random.h"

int randInt(int min, int max)
{
	return rand() % ((max+1)-min) + min;
}

double randFloat(double min, double max)
{
        int i = rand();
        double d = (double)i / RAND_MAX;
        return d * (max-min) + min;
}

void exportRandom()
{
	using namespace boost::python;
	pythonAddFunction("randint", randInt);
	pythonAddFunction("randfloat", randFloat);
}

