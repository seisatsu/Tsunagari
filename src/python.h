/*********************************
** Tsunagari Tile Engine        **
** python.h                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

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

#ifndef PYTHON_H
#define PYTHON_H

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

extern int inPythonScript;

//! Initialize Python libraries for use.
bool pythonInit();

//! Free resources used by Python libraries and uninitialize them.
void pythonFinalize();

//! Print last error received within Python.
void pythonErr();


//! Access to global namespace shared by all Python scripts.
boost::python::object pythonGlobals();

//! Convenience function for binding a C++ object into the global Python
//! namespace.
template<class T>
void pythonSetGlobal(const char* name, T pointer)
{
	try {
		pythonGlobals()[name] = boost::python::ptr(pointer);
	} catch (boost::python::error_already_set) {
		pythonErr();
	}
}

template<class Fn>
void pythonAddFunction(const char* name, Fn fn)
{
	using namespace boost::python;

	try {
		scope bltins(import("__builtin__"));
		def(name, fn);
	} catch (error_already_set) {
		pythonErr();
	}
}


//! Compile a Python script. Must provide both a representative filename for
//! any error messages along with a string containing the body of code to
//! compile. Returns NULL on failure and prints any errors.
PyCodeObject* pythonCompile(const char* fn, const char* code);

//! Run a compiled Python script. Returns false on runtime error and prints the
//! error.
bool pythonExec(PyCodeObject* code);

#endif

