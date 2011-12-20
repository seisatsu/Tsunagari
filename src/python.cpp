/******************************
** Tsunagari Tile Engine     **
** python.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <boost/format.hpp>
#include <stdlib.h> // for exit
#include <string.h> // for strrchr

#include "log.h"
#include "python.h"
#include "resourcer.h"

void pythonErr()
{
	// Something bad happened. Error is already set in Python.
	PyObject *ptype, *pvalue, *ptraceback;
	PyErr_Fetch(&ptype, &pvalue, &ptraceback);

	char* type = PyExceptionClass_Name(ptype);
	type = strrchr(type, '.') + 1;
	char* value = PyString_AsString(pvalue);

	Log::err("Python", boost::str(
		boost::format("%s: %s") % type % value
	));
	exit(1);
}

void pythonInit()
{
	try {
		preinitPythonResourcer();
		Py_Initialize();
		postinitPythonResourcer();
	} catch (boost::python::error_already_set) {
		Log::err("Python", "An error occured while populating the "
			           "Python modules:");
		pythonErr();
	}
}

void pythonFinalize()
{
	Py_Finalize();
}

python::object pyGlobals()
{
	python::object main = python::import("__main__");
	python::object global = main.attr("__dict__");
	return global;
}

void pyIncludeModule(const char* name)
{
	python::object module(
		python::handle<>(PyImport_ImportModule(name))
	);
	pyGlobals()[name] = module;
}

void pyExec(const char* s)
{
	try {
		python::exec(s, pyGlobals(), pyGlobals());
	} catch (boost::python::error_already_set) {
		pythonErr();
	}
}

