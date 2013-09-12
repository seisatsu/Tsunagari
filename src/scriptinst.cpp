/***************************************
** Tsunagari Tile Engine              **
** scriptinst.cpp                     **
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
# include <stdio.h> // Mac needs ssize_t defined for pyport.h
#endif

// from Python
#ifdef __APPLE__
# include <pyport.h>
# include <object.h>
#endif
#include <import.h>

#include "log.h"
#include "python.h"
#include "reader.h"
#include "scriptinst.h"


struct validate_visitor : public boost::static_visitor<bool>
{
	bool operator()(BytecodeRef bc) const
	{
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
	}

	bool operator()(boost::python::object) const
	{
		return true;
	}
};


struct invoke_visitor : public boost::static_visitor<bool>
{
	bool operator()(BytecodeRef bc) const
	{
		return (bc && bc->valid()) ? bc->execute() : false;
	}

	bool operator()(boost::python::object callable) const
	{
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
	}
};



ScriptInst::ScriptInst(const std::string& source)
	: data(Reader::getBytecode(source))
{
	if (!validate()) {
		Log::err("ScriptInst", "Error loading " + source);
	}
}


ScriptInst::ScriptInst(boost::python::object callable)
	: data(callable)
{
}


bool ScriptInst::validate()
{
	return boost::apply_visitor(validate_visitor(), data);
}


bool ScriptInst::invoke()
{
	return boost::apply_visitor(invoke_visitor(), data);
}


struct topython_visitor : public boost::static_visitor<PyObject*>
{
	PyObject* operator()(BytecodeRef bc) const
	{
		boost::python::object str;
		if (bc)
			str = boost::python::object(bc->filename());
		else
			str = boost::python::object("");
		return boost::python::incref(str.ptr());
	}

	PyObject* operator()(boost::python::object callable) const
	{
		return boost::python::incref(callable.ptr());
	}
};


struct scriptinst_to_python
{
	static PyObject* convert(ScriptInst script)
	{
		return boost::apply_visitor(topython_visitor(), script.data);
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

