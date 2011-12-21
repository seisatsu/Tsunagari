/******************************
** Tsunagari Tile Engine     **
** python.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef PYTHON_H
#define PYTHON_H

#include <stdlib.h>

#include <boost/python.hpp>

void pythonInit();
void pythonFinalize();
void pythonErr();
boost::python::object pyGlobals();

template<class T>
void pySetGlobal(const char* name, T pointer)
{
	try {
		pyGlobals()[name] = boost::python::ptr(pointer);
	} catch (boost::python::error_already_set) {
		pythonErr();
	}
}

void pyIncludeModule(const char* name);
void pyExec(const char* s);

#endif

