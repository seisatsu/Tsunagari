#ifndef SCRIPTINST_H
#define SCRIPTINST_H

#include <string>

#include <boost/python/object.hpp>
#include <boost/variant.hpp>

class ScriptInst
{
public:
	ScriptInst();
	ScriptInst(const std::string& strloc);
	ScriptInst(boost::python::object pyfn);

	// context = domain for errors msgs
	bool validate(const std::string& context);
	bool invoke();

private:
	struct strref {
		std::string filename;
		std::string funcname;
		PyCodeObject* funccall;
	};

	boost::variant<
		void*,
		strref,
		boost::python::object
	> data;

	friend struct validate_visitor;
	friend struct invoke_visitor;
};

#endif
