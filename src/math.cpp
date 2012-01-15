/*********************************
** Tsunagari Tile Engine        **
** math.cpp                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include "common.h"
#include "math.h"
#include "python.h"

using boost::python::class_;
using boost::python::init;
using boost::python::other;
using boost::python::self;

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

