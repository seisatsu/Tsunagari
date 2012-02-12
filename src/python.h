/*********************************
** Tsunagari Tile Engine        **
** python.h                     **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef PYTHON_H
#define PYTHON_H

// In this file.
#include <boost/python/errors.hpp>
#include <boost/python/object.hpp>
#include <boost/python/ptr.hpp>

// For bindings.
#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/other.hpp>
#include <boost/python/self.hpp>

//! Initialize Python libraries for use.
bool pythonInit();

//! Free resourcers used by Python libraries and uninitialize them.
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


//! Compile a Python script. Must provide both a representative filename for
//! any error messages along with a string containing the body of code to
//! compile. Returns NULL on failure and prints any errors.
PyCodeObject* pythonCompile(const char* fn, const char* code);

//! Run a compiled Python script. Returns false on runtime error and prints the
//! error.
bool pythonExec(PyCodeObject* code);

#endif

