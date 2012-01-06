/*********************************
** Tsunagari Tile Engine        **
** python.cpp                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <string.h> // for strrchr

#include <boost/format.hpp>

// Python includes.
#include <Python.h>
#include <grammar.h> // for struct grammar
#include <node.h> // for struct node
#include <parsetok.h> // for PyParser_ParseStringFlags

#include "log.h"
#include "python.h"
#include "python_bindings.h" // for pythonInitBindings

namespace bp = boost::python;

static const char* moduleWhitelist[] = {
	"math",
	"sys",
	NULL,
};


static void pythonUndefineBuiltin(const char* key)
{
	bp::object k(key);
	PyDict_DelItem(pythonBuiltins().ptr(), k.ptr());
	if (PyErr_Occurred())
		bp::throw_error_already_set();
}

static void pythonIncludeModule(const char* name)
{
	bp::object module(bp::handle<>(PyImport_ImportModule(name)));
	pythonGlobals()[name] = module;
}

static void pythonSetDefaultEncoding(const char* enc)
{
	if (PyUnicode_SetDefaultEncoding(enc) != 0) {
		PyErr_Format(PyExc_SystemError,
			"encoding %s not found", enc);
		bp::throw_error_already_set();
	}
}

static PyObject*
safeImport(PyObject*, PyObject* args, PyObject* kwds)
{
	static const char* kwlist[] = {"name", "globals", "locals", "fromlist",
		"level", 0};
	char* name;
	PyObject* globals, *locals, *fromList;
	int level = -1;

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|OOOi:__import__",
			(char**)kwlist, &name, &globals, &locals, &fromList,
			level))
		return NULL;
	Log::dbg("Python", std::string("import ") + name);

	bool whitelisted = false;
	for (int i = 0; moduleWhitelist[i]; i++) {
		const char* mod = moduleWhitelist[i];
		if (!strcmp(mod, name)) {
			whitelisted = true;
			break;
		}
	}
	if (whitelisted) {
		return PyImport_ImportModuleLevel(name, globals, locals,
			fromList, level);
	}
	else {
		PyErr_Format(PyExc_SystemError, "importing of most external "
			"modules disabled in Tsunagari");
		return NULL;
	}
}

static PyMethodDef newImport[] = {
	{"__import__", (PyCFunction)safeImport, METH_VARARGS | METH_KEYWORDS, ""},
	{NULL, NULL, 0, NULL},
};

static void pythonOverrideImportStatement()
{
	// InitModule doesn't delete existing modules, so we can use it to
	// insert new methods into a pre-existing module.
	PyObject* module = Py_InitModule("__builtin__", newImport);
	if (!module) {
		PyErr_Format(PyExc_SystemError, "overriding __builtin__ module failed");
		bp::throw_error_already_set();
	}
}

bool pythonInit()
{
	try {
		PyImport_AppendInittab("tsunagari", &pythonInitBindings);
		Py_Initialize();
		pythonSetDefaultEncoding("utf-8");
		pythonIncludeModule("tsunagari");

		// Hack in some rough safety. Disable external scripts and IO.
		pythonUndefineBuiltin("__import__");
		pythonUndefineBuiltin("execfile");
		pythonUndefineBuiltin("file");
		pythonUndefineBuiltin("open");
		pythonUndefineBuiltin("reload");
		// TODO: save "bytes = os.urandom(n)" maybe?

		pythonOverrideImportStatement();
	} catch (bp::error_already_set) {
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

	Log::err("Python", boost::str(boost::format("%s: %s") % type % value));
}

bp::object pythonBuiltins()
{
	static bool init = false;
	static bp::object bltins;
	if (!init) {
		init = true;
		bltins = bp::import("__builtin__").attr("__dict__");
	}
	return bltins;
}

bp::object pythonGlobals()
{
	static bool init = false;
	static bp::object global;
	if (!init) {
		init = true;
		global = bp::import("__main__").attr("__dict__");
	}
	return global;
}

extern grammar _PyParser_Grammar; // From Python's graminit.c

PyCodeObject* pythonCompile(const char* fn, const char* code)
{
	// XXX: memory leaks
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

