/*********************************
** Tsunagari Tile Engine        **
** vec.cpp                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <boost/python.hpp>

#include "vec.h"

struct vicoord_to_python_tuple
{
	static PyObject* convert(const vicoord& c)
	{
		using namespace boost::python;

		object tuple = make_tuple(c.x, c.y, c.z);
		return incref(tuple.ptr());
	}
};

void exportVecs()
{
	using namespace boost::python;

	to_python_converter<vicoord, vicoord_to_python_tuple>();

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
}

