/*********************************
** Tsunagari Tile Engine        **
** log.cpp                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <iostream>
#include "config.h"
#include "log.h"
#include "python.h"

static verbosity_t verb = V_NORMAL;

static std::string& chomp(std::string& str)
{
	std::string::size_type notwhite = str.find_last_not_of(" \t\n\r");
	str.erase(notwhite + 1);
	return str;
}

void Log::setVerbosity(verbosity_t v)
{
	verb = v;
}

void Log::info(std::string domain, std::string msg)
{
	std::string str = "Info [" + domain + "] - " + chomp(msg);
	if (verb > V_NORMAL)
		std::cout << str << std::endl;
}

void Log::err(std::string domain, std::string msg)
{
	std::string str = "Error [" + domain + "] - " + chomp(msg);
	if (inPythonScript) {
		PyErr_SetString(PyExc_RuntimeError, str.c_str());
		boost::python::throw_error_already_set();
	}
	else {
		if (verb > V_QUIET)
			std::cerr << str << std::endl;
	}
}

void Log::fatal(std::string domain, std::string msg)
{
	std::string str = "Fatal [" + domain + "] - " + chomp(msg);
	if (inPythonScript) {
		PyErr_SetString(PyExc_RuntimeError, str.c_str());
		boost::python::throw_error_already_set();
	}
	else {
		std::cerr << str << std::endl;
	}
}

