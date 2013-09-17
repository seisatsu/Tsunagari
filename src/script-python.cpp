/***************************************
** Tsunagari Tile Engine              **
** script-python.cpp                  **
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
#include "python.h"
#include "reader.h"
#include "script-python.h"


template<>
ScriptRef Script::create(std::string source)
{
	std::string s_file, s_func;
	size_t colon;
	PyObject* module = NULL;
	PythonScript* script;

	colon = source.find(':');
	if (colon != std::string::npos) {
		s_file = source.substr(0, colon);
		s_func = source.substr(colon + 1);
	}
	else {
		s_file = source;
	}

	if ((module = PyImport_ImportModule(s_file.c_str())) == NULL) {
		Log::err("Script", s_file + ": import failed");
		goto err;
	}

	script = new PythonScript;
	script->module = module;
	script->function = s_func;
	return ScriptRef(script);

err:
	pythonErr();

	Py_XDECREF(module);
	return ScriptRef();
}


template<>
ScriptRef Script::create(const char* source)
{
	return Script::create(std::string(source));
}


PythonScript::PythonScript()
	: module(NULL)
{
}


PythonScript::~PythonScript()
{
	Py_DECREF(module);
}


bool PythonScript::validate()
{
	// Always valid...?
	return true;
}


bool PythonScript::invoke()
{
	assert(function.size());

	PyObject* result = NULL;

	result = PyObject_CallMethod(module, (char*)function.c_str(), (char*)"()");
	if (result == NULL) {
		pythonErr();
		return false;
	}
	Py_DECREF(result);
	return true;
}


/* The code below has support for being bound from inside of Python


ScriptInst::ScriptInst(const std::string& source)
: dataType(BYTECODE_REF), data(Reader::getBytecode(source))
{
	if (!validate()) {
		Log::err("ScriptInst", "Error loading " + source);
	}
}


ScriptInst::ScriptInst(boost::python::object& callable)
: dataType(BOOST_PY_OBJ), data(callable)
{
}


ScriptInst::ScriptInst(const ScriptInst& s)
: dataType(s.dataType)
{
	switch (dataType) {
		case BYTECODE_REF:
			data = s.data.bcr;
			break;
		case BOOST_PY_OBJ:
			data = s.data.bpo;
			break;
	}
}


ScriptInst::~ScriptInst()
{
	switch (dataType) {
		case BYTECODE_REF:
			data = s.data.bcr;
			break;
		case BOOST_PY_OBJ:
			data = s.data.bpo;
			break;
	}
}


bool ScriptInst::validate()
{
	BytecodeRef& bc = *(BytecodeRef*)data;

	switch (dataType) {
		case BYTECODE_REF:
			if (!bc) {
				Log::err("ScriptInst", "<null script>: script not valid");
				return false;
			}
			if (!bc->valid()) {
				Log::err("ScriptInst", bc->filename() +
						 ": script not valid");
				return false;
			}
			return true;
		case BOOST_PY_OBJ:
			return true;
		default:
			Log::fatal("ScriptInstInternal", "validate(): unknown data type");
			return false;
	}
}


bool ScriptInst::invoke()
{
	BytecodeRef& bc = *(BytecodeRef*)data;
	boost::python::object& callable = *(boost::python::object*)data;

	switch (dataType) {
		case BYTECODE_REF:
			return (bc && bc->valid()) ? bc->execute() : false;
		case BOOST_PY_OBJ:
			try {
				inPythonScript++;
				callable();
				inPythonScript--;
				return true;
			} catch (boost::python::error_already_set) {
				inPythonScript--;
				// XXX: How does this interact with a C++/Python callstack?
				//Log::err("Python", "Originating from " + source + ":");
				pythonErr();
				return false;
			}
		default:
			Log::fatal("ScriptInstInternal", "invoke(): unknown data type");
			return false;
	}
}


struct scriptinst_to_python
{
	static PyObject* convert(ScriptInst script)
	{
		BytecodeRef& bc = *(BytecodeRef*)data;
		boost::python::object& callable = *(boost::python::object*)data;

		switch (dataType) {
			case BYTECODE_REF:
				boost::python::object str;
				if (bc)
					str = boost::python::object(bc->filename());
				else
					str = boost::python::object("");
				return boost::python::incref(str.ptr());
			case BOOST_PY_OBJ:
				return boost::python::incref(callable.ptr());
			default:
				Log::fatal("ScriptInstInternal", "to_python: convert(): unknown data type");
				return false;
		}
	}
};


struct scriptinst_from_python
{
	scriptinst_from_python()
	{
		boost::python::converter::registry::push_back(
													  &convertible,
													  &construct,
													  boost::python::type_id<ScriptInst>());
	}

	// Can this be converted to a ScriptInst?
	static void* convertible(PyObject* obj)
	{
		//bool callable = obj->ob_type->tp_call != NULL;
		//const char* tp_name = obj->ob_type->tp_name;
		// XXX: Return non-NULL only if string or
		// callable (fn or lambda?).
		return obj;
	}

	// Convert. boost::python provides us with a chunch of memory that we
	// have to construct in-place.
	static void construct(
						  PyObject* obj,
						  boost::python::converter::rvalue_from_python_stage1_data* data)
	{
		// Prevent compilation name collisions with "object" by making
		// it "bp::object".
		namespace bp = boost::python;

		void* storage =
		((bp::converter::rvalue_from_python_storage<ScriptInst>*)data)
		->storage.bytes;

		if (PyString_Check(obj)) {
			const char* value = PyString_AsString(obj);
			new (storage) ScriptInst(value);
		}
		else {
			// By default, the PyObject is a borrowed reference,
			// which means it hasn't been incref'd.
			bp::handle<> hndl(bp::borrowed(obj));
			new (storage) ScriptInst(bp::object(hndl));
		}

		data->convertible = storage;
	}
};


void exportScriptInst()
{
	using namespace boost::python;
	
	to_python_converter<ScriptInst, scriptinst_to_python>();
	scriptinst_from_python();
}

*/