/******************************
** Tsunagari Tile Engine     **
** python.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <stdlib.h> // for exit
#include <string.h> // for strrchr

#include <boost/format.hpp>

#include "log.h"
#include "python.h"
#include "resourcer.h"

namespace python = boost::python;

BOOST_PYTHON_MODULE(tsunagari)
{
	exportResourcer();
}


void pythonInit()
{
	try {
		PyImport_AppendInittab("tsunagari", &inittsunagari);
		Py_Initialize();
		pyIncludeModule("tsunagari");
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

