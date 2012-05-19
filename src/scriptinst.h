#ifndef SCRIPTINST_H
#define SCRIPTINST_H

#include <string>

#include <boost/python/object.hpp>
#include <boost/variant.hpp>

class ScriptInst
{
public:
	ScriptInst(const std::string& filename);
	ScriptInst(boost::python::object pyfn);

	bool invoke();

private:
	boost::variant<
		std::string,
		boost::python::object
	> data;
};

#endif
