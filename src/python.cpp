/*********************************
** Tsunagari Tile Engine        **
** python.cpp                   **
** Copyright 2011-2012 OmegaSDG **
*********************************/

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
#include "reader.h"
#include "window.h"

#include <Python-ast.h>


namespace bp = boost::python;

static bp::object modMain, modBltin;
static bp::object dictMain, dictBltin;

int inPythonScript = 0;

//! List of known safe Python modules allowed for importing.
static std::string moduleWhitelist[] = {
	"__builtin__",
	"__main__",
	"math",
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
	if (Reader::resourceExists(name)) {
		Reader::runPythonScript(name);
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

static PyObject* nullPrint(PyObject*, PyObject*)
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
	{"__import__", (PyCFunction)safeImport, METH_VARARGS | METH_KEYWORDS, ""},
	{"execfile", nullExecfile, METH_VARARGS, ""},
	{"file", nullFile, METH_VARARGS, ""},
	{"open", nullOpen, METH_VARARGS, ""},
	{"print", nullPrint, METH_VARARGS | METH_KEYWORDS, ""},
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
		bp::object traceback(import("traceback"));
		bp::object format_exception(traceback.attr("format_exception"));
		bp::object formatted_list(format_exception(hexc, hval, htb));
		bp::object formatted(str("").join(formatted_list));
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

	if (conf.halting == HALT_SCRIPT) {
		Log::fatal("Python", extractException(exc, val, tb));
		exit(1);
	}
	else
		Log::err("Python", extractException(exc, val, tb));
}

bp::object pythonGlobals()
{
	return dictMain;
}

PyCodeObject* pythonCompile(const char* fn, const char* code)
{
	PyArena *arena = PyArena_New();
	if (!arena) {
		return NULL;
	}

	mod_ty mod = PyParser_ASTFromString(code, fn, Py_file_input, NULL, arena);
	if (!mod) {
		Log::err("Python",
		         std::string(fn) + ": failed to parse");
		pythonErr();
		return NULL;
	}

	PyCodeObject* co = PyAST_Compile(mod, fn, NULL, arena);
	if (!co) {
		Log::err("Python",
		         std::string(fn) + ": failed to compile");
		pythonErr();
		return NULL;
	}

	PyArena_Free(arena);
	return co;
}

bool pythonExec(PyCodeObject* code)
{
	if (!code)
		return false;

	try {
		inPythonScript++;

		// FIXME: locals, globals
		PyObject* globals = dictMain.ptr();
		PyObject* result = PyEval_EvalCode(code, globals, globals);

		inPythonScript--;
		if (!result)
			pythonErr();
		return result;
	} catch (boost::python::error_already_set) {
		inPythonScript--;
		pythonErr();
		return NULL;
	}
}

