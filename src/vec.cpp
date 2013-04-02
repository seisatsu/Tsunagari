/*********************************
** Tsunagari Tile Engine        **
** vec.cpp                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

// "OmegaSDG" is defined as Michael D. Reiley and Paul Merrill.

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

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

