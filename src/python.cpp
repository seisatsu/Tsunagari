/***************************************
** Tsunagari Tile Engine              **
** python.cpp                         **
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

#ifdef __APPLE__
#include <Python.h>
#endif

#include <algorithm> // for std::replace
#include <signal.h> // for SIGINT and SIG_DFL
#include <string.h> // for strrchr

#include <boost/python.hpp>

#include "client-conf.h"
#include "log.h"
#include "python.h"
#include "python-bindings.h" // for pythonInitBindings
#include "pyworldfinder.h"
#include "reader.h"
#include "window.h"

#include <Python-ast.h>


namespace bp = boost::python;

static PyObject* mainModule;
static PyObject* mainDict;

int inPythonScript = 0;


static PyObject* nullExecfile(PyObject*, PyObject*)
{
	PyErr_SetString(PyExc_RuntimeError,
	             "file(): Tsunagari does not allow accessing the standard filesystem");
	return NULL;
}

static PyObject* nullFile(PyObject*, PyObject*)
{
	PyErr_SetString(PyExc_RuntimeError,
	             "file(): Tsunagari does not allow accessing the standard filesystem");
	return NULL;
}

static PyObject* nullOpen(PyObject*, PyObject*)
{
	PyErr_SetString(PyExc_RuntimeError,
	             "open(): Tsunagari does not allow accessing the standard filesystem");
	return NULL;
}

static PyObject* nullPrint(PyObject*, PyObject*, PyObject*)
{
	PyErr_SetString(PyExc_RuntimeError,
	             "print(): Tsunagari does not allow scripts to print");
	return NULL;
}

static PyObject* nullReload(PyObject*, PyObject*)
{
	PyErr_SetString(PyExc_RuntimeError,
	             "reload(): Tsunagari does not allow module reloading");
	return NULL;
}

PyMethodDef nullMethods[] = {
	{"execfile", (PyCFunction)nullExecfile, METH_VARARGS, ""},
	{"file", (PyCFunction)nullFile, METH_VARARGS, ""},
	{"open", (PyCFunction)nullOpen, METH_VARARGS, ""},
	{"print", (PyCFunction)nullPrint, METH_VARARGS | METH_KEYWORDS, ""},
	{"reload", (PyCFunction)nullReload, METH_O, ""},
	{NULL, NULL, 0, NULL}
};

static bool sysPathAppend(const std::string& path)
{
	PyObject* pypath = NULL;
	PyObject* syspath = NULL;
	int idx = -1;

	pypath = PyString_FromString(path.c_str());
	if (pypath == NULL)
		goto err;

	syspath = PySys_GetObject((char*)"path");
	if (syspath == NULL || !PyList_Check(syspath)) {
		Log::fatal("Python",
			"sys.path must be a list of strings");
		goto err;
	}

	idx = PyList_Append(syspath, pypath);
	if (idx == -1) {
		Log::fatal("Python", "failed to append to sys.path");
		goto err;
	}

	Py_DECREF(pypath);
	return true;

err:
	Py_XDECREF(pypath);
	return false;
}

bool pythonInit()
{
	PyObject* name = NULL;
	PyObject* tsuModule = NULL;

	PyImport_AppendInittab("tsunagari", &pythonInitBindings);
	Py_InitializeEx(0);

	if (PyUnicode_SetDefaultEncoding("utf-8")) {
		PyErr_Format(PyExc_SystemError,
			"encoding %s not found", "utf-8");
		goto err;
	}

	if ((mainModule = PyImport_ImportModule("__main__")) == NULL)
		goto err;
	if ((mainDict = PyModule_GetDict(mainModule)) == NULL)
		goto err;

	if ((name = PyString_FromString("tsunagari")) == NULL)
		goto err;
	if ((tsuModule = PyImport_ImportModule("tsunagari")) == NULL)
		goto err;
	if (PyObject_SetAttr(mainModule, name, tsuModule) < 0)
		goto err;

	Py_DECREF(name);
	Py_DECREF(tsuModule);

	// Disable builtin filesystem IO.
	if (Py_InitModule("__builtin__", nullMethods) == NULL)
		goto err;

	// Disable most Python system imports.
	if (!add_worldfinder())
		goto err;

	// Add world to Python's sys.path.
	if (!sysPathAppend(BASE_ZIP_PATH))
		goto err;
	
	for (Conf::StringVector::iterator it = conf.dataPath.begin(); it != conf.dataPath.end(); it++)
		if (!sysPathAppend(*it))
			goto err;

	return true;

err:
	Log::fatal("Python", "An error occured while populating the "
			   "Python modules:");
	Log::setVerbosity(V_NORMAL); // Assure message can be seen.
	pythonErr();
	return false;
}

void pythonFinalize()
{
	Py_DECREF(mainModule);
	Py_Finalize();
}

static std::string extractTracebackWLib(PyObject* exc, PyObject* val,
                                        PyObject *tb)
{
	using namespace boost::python;

	handle<> hexc(exc), hval(allow_null(val)), htb(allow_null(tb));
	try {
		// format_exception can itself fail
		bp::object traceback(import("traceback"));
		bp::object format_exception(traceback.attr("format_exception"));
		bp::object formatted_list(format_exception(hexc, hval, htb));
		bp::object formatted(str("").join(formatted_list));
		return extract<std::string>(formatted);
	}
	catch (bp::error_already_set) {
		return std::string();
	}
}

static std::string extractException(PyObject* exc, PyObject* val, PyObject* tb)
{
	if (!val)
		return PyObject_REPR(exc);

	std::string result = extractTracebackWLib(exc, val, tb);
	if (result.size())
		return result;

	// This is bad. Python's exception handler failed.

	// FIXME: repr() escapes the string. Not what we want. Maybe str()? Will
	//   need to include exc then b/c type is removed.
	return PyObject_REPR(val);
}

/*
static std::string extractException2(PyObject* exc, PyObject* val, PyObject*)
{
	char* type = PyExceptionClass_Name(exc);
	char* dot = strrchr(type, '.');
	if (dot)
		type = dot + 1;
	char* value = PyString_AsString(val);

	std::string msg = "";
	msg += type ? type : "<unknown type>";
	if (value) {
		msg += ": ";
		msg += value;
	}
	return msg;
}
*/

void pythonErr()
{
	// Something bad happened. Error is already set in Python.
	PyObject* exc, *val, *tb;
	PyErr_Fetch(&exc, &val, &tb);

	if (!exc) {
		Log::err("Python",
			"pythonErr() called, but no exception set");
		return;
	}

	PyErr_NormalizeException(&exc, &val, &tb);

	if (conf.halting == HALT_SCRIPT) {
		Log::fatal("Python", extractException(exc, val, tb));
		exit(1);
	}
	else
		Log::err("Python", extractException(exc, val, tb));
}

PyObject* pythonGlobals()
{
	return mainDict;
}

