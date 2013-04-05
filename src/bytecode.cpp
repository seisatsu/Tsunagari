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

#include <assert.h>
#include <boost/python.hpp>
#include <Python-ast.h>

#include "bytecode.h"
#include "log.h"
#include "python.h"


static PyCodeObject* compile(const std::string& fn, const std::string& code)
{
	PyArena *arena = PyArena_New();
	if (!arena) {
		Log::err("Python", fn + ": failed to create arena");
		return NULL;
	}

	mod_ty mod = PyParser_ASTFromString(code.c_str(), fn.c_str(),
	                                    Py_file_input, NULL, arena);
	if (!mod) {
		Log::err("Python", fn + ": failed to parse");
		pythonErr();
		return NULL;
	}

	PyCodeObject* co = PyAST_Compile(mod, fn.c_str(), NULL, arena);
	if (!co) {
		Log::err("Python", fn + ": failed to compile");
		pythonErr();
		return NULL;
	}

	PyArena_Free(arena);
	return co;
}


static bool executeBytecode(PyCodeObject* code, PyObject* globals, PyObject* locals)
{
	assert(code && globals && locals);

	inPythonScript++;
	PyObject* result = PyEval_EvalCode(code, globals, locals);
	inPythonScript--;

	if (!result)
		pythonErr();
	return result;
}


Bytecode::Bytecode(const std::string& filename,
                   const std::string& sourcecode)
	: fn(filename),
	  code(compile(filename, sourcecode)),
	  locals(PyDict_New())
{
}


Bytecode::~Bytecode()
{
	Py_XDECREF(code);
	Py_XDECREF(locals);
}


bool Bytecode::execute()
{
	assert(valid());

	PyObject* globals = pythonGlobals();
	Log::err("foo", PyString_AsString(PyObject_Repr(globals)));
	return executeBytecode(code, globals, locals);
}


bool Bytecode::valid() const
{
	return code && locals;
}


const std::string& Bytecode::filename() const
{
	return fn;
}

