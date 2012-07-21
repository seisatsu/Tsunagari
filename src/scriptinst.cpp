#include "log.h"
#include "python.h"
#include "resourcer.h"
#include "scriptinst.h"

struct validate_visitor : public boost::static_visitor<bool>
{
	std::string context;

	validate_visitor(const std::string& context) : context(context) {}

	bool operator()(void*) const
	{
		return true;
	}

	bool operator()(ScriptInst::strref ref) const
	{
		Resourcer* rc = Resourcer::instance();

		if (ref.filename.empty()) {
			Log::err(context,
				"script filename is empty");
			return false;
		}

		if (!rc->resourceExists(ref.filename)) {
			Log::err(context,
				ref.filename + "script file not found");
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
	bool operator()(void*) const
	{
		return true;
	}

	bool operator()(ScriptInst::strref ref) const
	{
		Resourcer* rc = Resourcer::instance();

		if (ref.funcname.size()) {
			rc->runPythonScript(ref.filename);
			return pythonExec(ref.funccall);
		}
		else {
			return rc->runPythonScript(ref.filename);
		}
	}

	bool operator()(boost::python::object pyfn) const
	{
		try {
			inPythonScript++;
			pyfn();
			inPythonScript--;
			return true;
		} catch (boost::python::error_already_set) {
			inPythonScript--;
			pythonErr();
			return false;
		}
	}
};

ScriptInst::ScriptInst()
	: data((void*)NULL)
{
}

ScriptInst::ScriptInst(const std::string& strloc)
{
	size_t colon = strloc.find(':');
	strref ref;
	if (colon == std::string::npos) {
		ref.filename = strloc;
	}
	else {
		ref.filename = strloc.substr(0, colon);
		ref.funcname = strloc.substr(colon + 1);
		ref.funccall = pythonCompile(
			"<Tsunagari trigger>",
			(ref.funcname).c_str()
		);
	}
	data = ref;
}

ScriptInst::ScriptInst(boost::python::object pyfn)
	: data(pyfn)
{
}

bool ScriptInst::validate(const std::string& context)
{
	return boost::apply_visitor(validate_visitor(context), data);
}

bool ScriptInst::invoke()
{
	return boost::apply_visitor(invoke_visitor(), data);
}








struct topython_visitor : public boost::static_visitor<PyObject*>
{
	PyObject* operator()(void*) const
	{
		using namespace boost::python;

		return incref(Py_None);
	}

	PyObject* operator()(ScriptInst::strref ref) const
	{
		using namespace boost::python;

		// Prevent compilation name collisions with "object" by making
		// it "bp::object".
		namespace bp = boost::python;

		bp::object str;
		if (ref.funcname.size())
			str = bp::object(ref.filename + ":" + ref.funcname);
		else
			str = bp::object(ref.filename);
		return incref(str.ptr());
	}

	PyObject* operator()(boost::python::object pyfn) const
	{
		using namespace boost::python;

		return incref(pyfn.ptr());
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
		using namespace boost::python;

		// Prevent compilation name collisions with "object" by making
		// it "bp::object".
		namespace bp = boost::python;

		void* storage =
			((converter::rvalue_from_python_storage<ScriptInst>*)data)
				->storage.bytes;

		if (PyString_Check(obj)) {
			const char* value = PyString_AsString(obj);
			new (storage) ScriptInst(value);
		}
		else {
			// By default, the PyObject is a borrowed reference,
			// which means it hasn't been incref'd.
			handle<> hndl(borrowed(obj));
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

