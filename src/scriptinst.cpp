#include "log.h"
#include "python.h"
#include "resourcer.h"
#include "scriptinst.h"

struct invoke_visitor : public boost::static_visitor<bool>
{
	bool operator()(std::string filename) const
	{
		if (filename.empty()) {
			Log::err("Script",
				"trying to run script, but filename is empty");
			return false;
		}

		Resourcer* rc = Resourcer::instance();
		return rc->runPythonScript(filename);
	}

	bool operator()(boost::python::object pyfn) const
	{
		try {
			pyfn();
			return true;
		} catch (boost::python::error_already_set) {
			pythonErr();
			return false;
		}
	}
};

ScriptInst::ScriptInst(const std::string& filename)
	: data(filename)
{
}

ScriptInst::ScriptInst(boost::python::object pyfn)
	: data(pyfn)
{
}

bool ScriptInst::invoke()
{
	return boost::apply_visitor(invoke_visitor(), data);
}
