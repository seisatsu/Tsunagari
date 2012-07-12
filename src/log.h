/*********************************
** Tsunagari Tile Engine        **
** log.h                        **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef LOG_H
#define LOG_H

#include <boost/utility.hpp> // for boost::noncopyable
#include <string>

#include "timer.h"

enum verbosity_t {
	V_QUIET,   // Display fatals.
	V_NORMAL,  // Display fatals and errors. (This is the default.)
	V_VERBOSE  // Display fatals, errors and info.
};

class Log : boost::noncopyable
{
public:
	static void setVerbosity(verbosity_t mode);

	static void info(std::string domain, std::string msg);
	static void err(std::string domain, std::string msg);
	static void fatal(std::string domain, std::string msg);

	static Timer* tsTimer;

private:
	Log();
	~Log();
};

#endif

