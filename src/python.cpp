/*********************************
** Tsunagari Tile Engine        **
** python.cpp                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <algorithm> // for std::replace
#include <string.h> // for strrchr

#include <boost/python.hpp>

#include "log.h"
#include "python.h"
#include "python_bindings.h" // for pythonInitBindings
#include "resourcer.h"

// Include libpython after everything else so it doesn't mess with Windows'
// namespace.
#include <Python.h>
#include <grammar.h> // for struct grammar
#include <node.h> // for struct node
#include <parsetok.h> // for PyParser_ParseStringFlags


namespace bp = boost::python;

static Resourcer* rc = NULL;
static bp::object modMain, modBltin;
static bp::object dictMain, dictBltin;


//! List of known safe Python modules allowed for importing.
static std::string moduleWhitelist[] = {
	"__builtin__",
	"__main__",
	"math",
	"sys",
	"",
	// TODO: save "os.urandom(n)" maybe?
};

static bool inWhitelist(const std::string& name)
{
	for (int i = 0; moduleWhitelist[i].size(); i++)
		if (name == moduleWhitelist[i])
			return true;
	return false;
}


static void pythonUndefineBuiltin(const char* key)
{
	bp::object k(key);
	PyDict_DelItem(dictBltin.ptr(), k.ptr());
	if (PyErr_Occurred())
		bp::throw_error_already_set();
}

static void pythonIncludeModule(const char* name)
{
	bp::object module(bp::handle<>(PyImport_ImportModule(name)));
	dictMain[name] = module;
}

static void pythonSetDefaultEncoding(const char* enc)
{
	if (PyUnicode_SetDefaultEncoding(enc) != 0) {
		PyErr_Format(PyExc_SystemError,
			"encoding %s not found", enc);
		bp::throw_error_already_set();
	}
}

//! Python will call this function when it tries to import a module. Things are
//! a bit messy as we conform to the Python ABI.
static PyObject*
safeImport(PyObject*, PyObject* args, PyObject* kwds)
{
	static const char* kwlist[] = {"name", "globals", "locals", "fromlist",
		"level", 0};
	char* _name;
	std::string name;
	PyObject* globals, *locals, *fromList;
	int level = -1;

	// Validate args from Python.
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|OOOi:__import__",
			(char**)kwlist, &_name, &globals, &locals, &fromList,
			&level))
		return NULL;
	name = _name;
	Log::dbg("Python", "import " + name);

	// Search whitelisted Python modules.
	if (inWhitelist(name))
		return PyImport_ImportModuleLevel(_name, globals, locals,
			fromList, level);

	// Search Python scripts inside World.
	std::replace(name.begin(), name.end(), '.', '/');
	name += ".py";
	if (rc && rc->resourceExists(name)) {
		rc->runPythonScript(name);
		return modMain.ptr(); // We have to return a module...
	}

	// Nothing acceptable found.
	std::string msg = std::string("Module '") + _name + "' not found or "
		"not allowed. Note that Tsunagari runs in a sandbox and does "
		"not allow most external modules.";
	PyErr_Format(PyExc_ImportError, msg.c_str());
	return NULL;
}

static void pythonOverrideImportStatement()
{
	static PyMethodDef newImport[] = {
		{"__import__", (PyCFunction)safeImport,
		 METH_VARARGS | METH_KEYWORDS, ""},
		{NULL, NULL, 0, NULL},
	};

	// InitModule doesn't remove existing modules, so we can use it to
	// insert new methods into a pre-existing module.
	PyObject* module = Py_InitModule("__builtin__", newImport);
	if (!module) {
		PyErr_Format(PyExc_SystemError,
			"overriding __builtin__ module failed");
		bp::throw_error_already_set();
	}
}

static void nullExecfile(std::string /* filename */)
{
	Log::err("Python", "execfile(): Tsunagari runs scripts in a sandbox "
	                   "and does not allow accessing the standard "
			   "filesystem");
}

static void nullFile(std::string /* filename */)
{
	Log::err("Python", "file(): Tsunagari runs scripts in a sandbox and "
	                   "does not allow accessing the standard filesystem");
}

static void nullOpen(std::string /* filename */)
{
	Log::err("Python", "open(): Tsunagari runs scripts in a sandbox and "
	                   "does not allow accessing the standard filesystem");
}

bool pythonInit()
{
	try {
		PyImport_AppendInittab("tsunagari", &pythonInitBindings);
		Py_Initialize();
		pythonSetDefaultEncoding("utf-8");

		modMain = bp::import("__main__");
		dictMain = modMain.attr("__dict__");
		modBltin = bp::import("__builtin__");
		dictBltin = modBltin.attr("__dict__");

		pythonIncludeModule("tsunagari");

		// Hack in some rough safety. Disable external scripts and IO.
		dictBltin["execfile"] = bp::make_function(nullExecfile);
		dictBltin["file"] = bp::make_function(nullFile);
		dictBltin["open"] = bp::make_function(nullOpen);
		pythonUndefineBuiltin("reload"); // FIXME: Trouble with this one...

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

	Log::err("Python", std::string(type) + ": " + value);
}

void pythonSetResourcer(Resourcer* r)
{
	rc = r;
}

bp::object pythonGlobals()
{
	return dictMain;
}

extern grammar _PyParser_Grammar; // From Python's graminit.c

PyCodeObject* pythonCompile(const char* fn, const char* code)
{
	// FIXME: memory leaks
	// XXX: there's already a compile function in Python somewhere
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
		Log::err("Python",
		         std::string(fn) + ": possibly unknown compile error");
		pythonErr();
		return NULL;
	}
	return pco;
}

bool pythonExec(PyCodeObject* code)
{
	if (!code)
		return false;
	PyObject* globals = dictMain.ptr();
	PyObject* result = PyEval_EvalCode(code, globals, globals);
	if (!result)
		pythonErr();
	return result;
}

