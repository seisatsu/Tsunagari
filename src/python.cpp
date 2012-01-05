/*********************************
** Tsunagari Tile Engine        **
** python.cpp                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <string.h> // for strrchr

#include <boost/format.hpp>

#include "log.h"
#include "python.h"
#include "python_bindings.h" // for inittsunagari

// Note: Including before "python.h" breaks Windows builds.
#include <Python.h>
#include <grammar.h> // for struct grammar
#include <node.h> // for struct node
#include <parsetok.h> // for PyParser_ParseStringFlags

namespace python = boost::python;

static void pythonUndefine(const char* fn)
{
	PyObject* bltins = pythonBuiltins().ptr();
	PyDict_DelItem(bltins, PyString_FromString(fn));
	if (PyErr_Occurred())
		python::throw_error_already_set();
}

static void pythonIncludeModule(const char* name)
{
	python::object module(
		python::handle<>(PyImport_ImportModule(name))
	);
	pythonGlobals()[name] = module;
}

bool pythonInit()
{
	try {
		PyImport_AppendInittab("tsunagari", &pythonInitBindings);
		Py_Initialize();
		pythonUndefine("execfile");
		pythonUndefine("open");
		pythonIncludeModule("tsunagari");
	} catch (python::error_already_set) {
		Log::err("Python", "An error occured while populating the "
			           "Python modules:");
		pythonErr();
		return false;
	}
	return true;
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
}

python::object pythonBuiltins()
{
	static bool init = false;
	static python::object bltins;
	if (!init) {
		init = true;
		bltins = python::import("__builtin__").attr("__dict__");
	}
	return bltins;
}

python::object pythonGlobals()
{
	static bool init = false;
	static python::object global;
	if (!init) {
		init = true;
		global = python::import("__main__").attr("__dict__");
	}
	return global;
}

extern grammar _PyParser_Grammar; // From Python's graminit.c

PyCodeObject* pythonCompile(const char* fn, const char* code)
{
	perrdetail err;
	node* n = PyParser_ParseStringFlagsFilename(
		code, fn, &_PyParser_Grammar,
		Py_file_input, &err, 0
	);
	if (!n) {
		PyParser_SetError(&err);
		pythonErr();
		return NULL;
	}
	PyCodeObject* pco = PyNode_Compile(n, fn);
	if (!pco) {
		Log::err("Python", boost::str(
			boost::format("%s: possibly unknown compile error") % fn
		));
		pythonErr();
		return NULL;
	}
	return pco;
}

bool pythonExec(PyCodeObject* code)
{
	if (!code)
		return false;
	PyObject* g = pythonGlobals().ptr();
	PyObject* result = PyEval_EvalCode(code, g, g);
	if (!result)
		pythonErr();
	return result;
}

