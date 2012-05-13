/*********************************
** Tsunagari Tile Engine        **
** python.cpp                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <algorithm> // for std::replace
#include <signal.h> // for SIGINT and SIG_DFL
#include <string.h> // for strrchr

#include <boost/python.hpp>

#include "log.h"
#include "python.h"
#include "python-bindings.h" // for pythonInitBindings
#include "resourcer.h"

// Include libpython after everything else so it doesn't mess with Windows'
// namespace.
#include <Python.h>
#include <grammar.h> // for struct grammar
#include <node.h> // for struct node
#include <parsetok.h> // for PyParser_ParseStringFlags


namespace bp = boost::python;

static bp::object modMain, modBltin;
static bp::object dictMain, dictBltin;

int inPythonScript = 0;

//! List of known safe Python modules allowed for importing.
static std::string moduleWhitelist[] = {
	"__builtin__",
	"__main__",
	"math",
	"time",
	"traceback",
	"",
};

static bool inWhitelist(const std::string& name)
{
	for (int i = 0; moduleWhitelist[i].size(); i++)
		if (name == moduleWhitelist[i])
			return true;
	return false;
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

	// Search whitelisted Python modules.
	if (inWhitelist(name))
		return PyImport_ImportModuleLevel(_name, globals, locals,
			fromList, level);

	Log::info("Python", "import " + name);

	// Search Python scripts inside World.
	std::replace(name.begin(), name.end(), '.', '/');
	name += ".py";
	Resourcer* rc = Resourcer::instance();
	if (rc->resourceExists(name)) {
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

static PyObject* nullExecfile(PyObject*, PyObject*)
{
	PyErr_SetString(PyExc_RuntimeError,
	             "file(): Tsunagari runs scripts in a sandbox and "
	             "does not allow accessing the standard filesystem");
	return NULL;
}

static PyObject* nullFile(PyObject*, PyObject*)
{
	PyErr_SetString(PyExc_RuntimeError,
	             "file(): Tsunagari runs scripts in a sandbox and "
	             "does not allow accessing the standard filesystem");
	return NULL;
}

static PyObject* nullOpen(PyObject*, PyObject*)
{
	PyErr_SetString(PyExc_RuntimeError,
	             "open(): Tsunagari runs scripts in a sandbox and "
	             "does not allow accessing the standard filesystem");
	return NULL;
}

static PyObject* nullReload(PyObject*, PyObject*)
{
	PyErr_SetString(PyExc_RuntimeError,
	             "reload(): Tsunagari does not allow module reloading");
	return NULL;
}

PyMethodDef nullMethods[] = {
	{"__import__", (PyCFunction)safeImport, METH_VARARGS | METH_KEYWORDS, ""},
	{"execfile", nullExecfile, METH_VARARGS, ""},
	{"file", nullFile, METH_VARARGS, ""},
	{"open", nullOpen, METH_VARARGS, ""},
	{"reload", nullReload, METH_O, ""},
	{NULL, NULL, 0, NULL},
};

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
		// InitModule doesn't remove existing modules, so we can use it to
		// insert new methods into a pre-existing module.
		PyObject* module = Py_InitModule("__builtin__", nullMethods);
		if (module == NULL)
			bp::throw_error_already_set();

		// Restore the default SIGINT handler.
		// Python messes with it. >:(
		PyOS_setsig(SIGINT, SIG_DFL);
	} catch (bp::error_already_set) {
		Log::fatal("Python", "An error occured while populating the "
			           "Python modules:");
		Log::setVerbosity(V_NORMAL); // Assure message can be seen.
		pythonErr();
		return false;
	}
	return true;
}

void pythonFinalize()
{
	Py_Finalize();
}

static std::string extractException(PyObject* exc, PyObject* val, PyObject* tb)
{
	using namespace boost::python;

	handle<> hexc(exc), hval(allow_null(val)), htb(allow_null(tb));
	if (!hval) {
		return extract<std::string>(str(hexc));
	}
	else {
		object traceback(import("traceback"));
		object format_exception(traceback.attr("format_exception"));
		object formatted_list(format_exception(hexc, hval, htb));
		object formatted(str("").join(formatted_list));
		return extract<std::string>(formatted);
	}
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

	Log::err("Python", extractException(exc, val, tb));
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
	inPythonScript++;
	PyObject* globals = dictMain.ptr();
	PyObject* result = PyEval_EvalCode(code, globals, globals);
	inPythonScript--;
	if (!result)
		pythonErr();
	return result;
}

