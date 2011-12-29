/******************************
** Tsunagari Tile Engine     **
** python.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef PYTHON_H
#define PYTHON_H

#include <stdlib.h>

#include <boost/python.hpp>

//! Initialize Python libraries for use.
void pythonInit();

//! Free resourcers used by Python libraries and uninitialize them.
void pythonFinalize();

//! Print last error received within Python.
void pythonErr();


//! Access to global namespace that is used by all Python scripts.
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


//! Compile a Python script. Returns NULL on failure and prints any errors.
PyCodeObject* pythonCompile(const char* fn, const char* code);

//! Run a compiled Python script. Print any errors.
void pythonExec(PyCodeObject* code);

#endif

