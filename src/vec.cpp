/*********************************
** Tsunagari Tile Engine        **
** vec.cpp                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include "common.h"
#include "python.h"
#include "vec.h"

using boost::python::class_;
using boost::python::init;
using boost::python::other;
using boost::python::self;

icube_t icube(int x1, int y1, int z1,
              int x2, int y2, int z2)
{
	icube_t c;
	c.x1 = x1;
	c.y1 = y1;
	c.z1 = z1;
	c.x2 = x2;
	c.y2 = y2;
	c.z2 = z2;
	return c;
}

void exportVecs()
{
	class_<icoord>("icoord", init<int, int, int>())
		.def_readwrite("x", &icoord::x)
		.def_readwrite("y", &icoord::y)
		.def_readwrite("z", &icoord::z)
		.def(self += other<icoord>())
		.def(self -= other<icoord>())
		.def(self *= other<icoord>())
		.def(self /= other<icoord>());
	class_<rcoord>("rcoord", init<double, double, double>())
		.def_readwrite("x", &rcoord::x)
		.def_readwrite("y", &rcoord::y)
		.def_readwrite("z", &rcoord::z)
		.def(self += other<rcoord>())
		.def(self -= other<rcoord>())
		.def(self *= other<rcoord>())
		.def(self /= other<rcoord>());
	class_<vicoord>("vicoord", init<int, int, double>())
		.def_readwrite("x", &vicoord::x)
		.def_readwrite("y", &vicoord::y)
		.def_readwrite("z", &vicoord::z);
}

