/*********************************
** Tsunagari Tile Engine        **
** log.cpp                      **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#include <iostream>

#include <Gosu/Timing.hpp>

#include "log.h"
#include "python.h"
#include "world.h"

#ifdef _WIN32
	#include <windows.h>
	#include "os-windows.h"
#endif

static verbosity_t verb = V_NORMAL;

static unsigned long startTime;

static std::string& chomp(std::string& str)
{
	std::string::size_type notwhite = str.find_last_not_of(" \t\n\r");
	str.erase(notwhite + 1);
	return str;
}

static std::string ts()
{
	unsigned long now = Gosu::milliseconds();

	std::ostringstream ts;
	ts.precision(3);
	ts << std::fixed;
	ts << (now - startTime) / (long double)1000.0;
	return "[" + ts.str() + "] ";
}

bool Log::init()
{
	startTime = Gosu::milliseconds();
	return true;
}

void Log::setVerbosity(verbosity_t v)
{
	verb = v;
}

void Log::info(std::string domain, std::string msg)
{
	std::string str = ts() + "Info [" + domain + "] - " + chomp(msg);
	if (verb > V_NORMAL)
		std::cout << str << std::endl;
}

void Log::err(std::string domain, std::string msg)
{
	std::string str = ts() + "Error [" + domain + "] - " + chomp(msg);
	if (inPythonScript) {
		PyErr_SetString(PyExc_RuntimeError, str.c_str());
		boost::python::throw_error_already_set();
	}
	else {
		if (verb > V_QUIET) {
			std::cerr << str << std::endl;
			#ifdef _WIN32
				wMessageBox("Tsunagari - Error", str);
			#endif
		}
	}
}

void Log::fatal(std::string domain, std::string msg)
{
	std::string str = ts() + "Fatal [" + domain + "] - " + chomp(msg);
	if (inPythonScript) {
		PyErr_SetString(PyExc_RuntimeError, str.c_str());
		boost::python::throw_error_already_set();
	}
	else {
		std::cerr << str << std::endl;
		#ifdef _WIN32
			wMessageBox("Tsunagari - Fatal", str);
		#endif
	}
}

void Log::reportVerbosityOnStartup()
{
	std::string verbString;
	switch (conf.verbosity)
	{
		case V_QUIET:
			verbString = "QUIET";
			break;
		case V_NORMAL:
			verbString = "NORMAL";
			break;
		case V_VERBOSE:
			verbString = "VERBOSE";
			break;
	}
	std::cout << ts() << "Reporting engine messages in " << verbString
			<< " mode." << std::endl;
}

static void pythonLogInfo(std::string msg)
{
	Log::info("Script", msg);
}

void exportLog()
{
	using namespace boost::python;

	pythonAddFunction("log", pythonLogInfo);
}

