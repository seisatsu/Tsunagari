/*********************************
** Tsunagari Tile Engine        **
** vec.cpp                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <boost/python.hpp>

#include "vec.h"

template<class T>
struct vec2_to_python_tuple
{
	static PyObject* convert(const T& v)
	{
		// Prevent compilation name collisions with "object" by making
		// it "bp::object".
		namespace bp = boost::python;

		bp::object tuple = bp::make_tuple(v.x, v.y);
		return bp::incref(tuple.ptr());
	}
};

template<class T>
struct vec3_to_python_tuple
{
	static PyObject* convert(const T& v)
	{
		// Prevent compilation name collisions with "object" by making
		// it "bp::object".
		namespace bp = boost::python;

		bp::object tuple = bp::make_tuple(v.x, v.y, v.z);
		return bp::incref(tuple.ptr());
	}
};

void exportVecs()
{
	using namespace boost::python;

	to_python_converter<ivec2, vec2_to_python_tuple<ivec2> >();
	to_python_converter<rvec2, vec2_to_python_tuple<rvec2> >();

	to_python_converter<ivec3, vec3_to_python_tuple<ivec3> >();
	to_python_converter<rvec3, vec3_to_python_tuple<rvec3> >();
	// typedef'd to ivec3 and rvec3, already registered
	/* to_python_converter<icoord, vec3_to_python_tuple<icoord> >(); */
	/* to_python_converter<rcoord, vec3_to_python_tuple<rcoord> >(); */

	// Not really a vec3, but we just need ::x, ::y, ::z members.
	to_python_converter<vicoord, vec3_to_python_tuple<vicoord> >();
}

