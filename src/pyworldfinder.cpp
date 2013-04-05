/***************************************
** Tsunagari Tile Engine              **
** pyworldfinder.cpp                  **
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


#include <Python.h>

#include "log.h"
#include "reader.h"


//! List of known safe Python modules allowed for importing.
static const char* module_whitelist[] = {
	"__builtin__",
	"__main__",
	"math",
	"traceback",
	NULL
};

static bool wf_in_whitelist(const char* name)
{
	for (int i = 0; module_whitelist[i]; i++)
		if (!strcmp(name, module_whitelist[i]))
			return true;
	return false;
}

static PyObject* wf_find_module(PyObject* /*self*/, PyObject* args)
{
	PyObject* fullname = NULL;
	PyObject* path = NULL; // dummy

	if (!PyArg_UnpackTuple(args, "find_module", 1, 2, &fullname, &path)) {
		// exc already set
		return NULL;
	}
	if (!PyString_Check(fullname)) {
		PyErr_Format(PyExc_TypeError, "expected string, got %.200s",
				fullname->ob_type->tp_name);
		return NULL;
	}

	const char* dotname = PyString_AsString(fullname); // "foo.bar"
	std::string slashname(dotname); // "foo/bar.py"
	std::replace(slashname.begin(), slashname.end(), '.', '/');

	// Returning Py_None allows the import to continue. NULL stops it.
	if (wf_in_whitelist(dotname)) {
		Py_INCREF(Py_None);
		return Py_None;
	}
	else if (Reader::directoryExists(slashname) && Reader::fileExists(slashname + "/__init__.py")) {
		Py_INCREF(Py_None);
		return Py_None;
	}
	else if (Reader::resourceExists(slashname + ".py")) {
		Py_INCREF(Py_None);
		return Py_None;
	}
	else {
		PyErr_Format(PyExc_ImportError, "No module name %.200s", dotname);
		return NULL;
	}
}

typedef struct {
	PyObject_HEAD
} worldfinderobject;

static PyMethodDef pyworldfinder_methods[] = {
	{"find_module", (PyCFunction)wf_find_module, METH_VARARGS, NULL},
	{NULL, NULL, 0, NULL},
};

static bool initted = false;
static PyTypeObject worldfinder_type = {
	PyObject_HEAD_INIT(&PyType_Type)
};

static bool wf_init_type()
{
	if (!initted) {
		initted = false;

		worldfinder_type.tp_name = "worldfinder";
		worldfinder_type.tp_basicsize = sizeof(worldfinderobject);
		worldfinder_type.tp_getattro = PyObject_GenericGetAttr;
		worldfinder_type.tp_flags = Py_TPFLAGS_DEFAULT;
		worldfinder_type.tp_methods = pyworldfinder_methods;

		if (PyType_Ready(&worldfinder_type) < 0) {
			Py_FatalError("Can't initialize worldfinder type");
			return false;
		}
	}
	return true;
}

static PyObject* wf_worldfinder_new()
{
	wf_init_type();
	return (PyObject*)PyObject_New(worldfinderobject, &worldfinder_type);
}


/** Public functions **/

bool add_worldfinder()
{
	PyObject* meta_path = NULL;
	PyObject* finder = NULL;
	int idx = -1;

	/* meta_path is a borrowed reference; no decref */
	meta_path = PySys_GetObject((char*)"meta_path");
	if (meta_path == NULL || !PyList_Check(meta_path)) {
		Log::fatal("Python",
			"sys.meta_path must be a list of import hooks");
		goto err;
	}

	finder = wf_worldfinder_new();
	if (finder == NULL) {
		Log::fatal("Python",
			"failed to create PyWorldFinder object");
		goto err;
	}

	idx = PyList_Append(meta_path, finder);
	if (idx == -1) {
		Log::fatal("Python", "failed to append to sys.meta_path");
		goto err;
	}

	Py_DECREF(finder);
	return true;

err:
	Py_XDECREF(finder);
	return false;
}

