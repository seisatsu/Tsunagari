/***************************************
** Tsunagari Tile Engine              **
** python-bindings-template.h         **
** Copyright 2011-2013 PariahSoft LLC **
***************************************/

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

#include <iostream> // Including this fixes a compilation-order error on
                    // XCode 4.6

// In this file.
#include <boost/python/def.hpp>
#include <boost/python/errors.hpp>
#include <boost/python/import.hpp>
#include <boost/python/object.hpp>
#include <boost/python/ptr.hpp>
#include <boost/python/scope.hpp>

// For bindings.
#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/other.hpp>
#include <boost/python/self.hpp>

#include "python.h"

//! Bind a C++ object into the global Python namespace.
template<class T>
void pythonSetGlobal(const std::string& name, T pointer)
{
	using namespace boost::python;

	object obj(ptr(pointer));
	PyObject* globals = NULL;

	if ((globals = pythonGlobals()) == NULL)
		goto err;
	PyDict_SetItemString(globals, name.c_str(), obj.ptr());

	return;

err:
	pythonErr();
}

template<class Fn>
void pythonAddFunction(const std::string& name, Fn fn)
{
	using namespace boost::python;
	
	try {
		scope bltins(import("__builtin__"));
		def(name.c_str(), fn);
	} catch (error_already_set) {
		pythonErr();
	}
}
