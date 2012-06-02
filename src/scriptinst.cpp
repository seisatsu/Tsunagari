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
