/*********************************
** Tsunagari Tile Engine        **
** log.h                        **
** Copyright 2011-2012 OmegaSDG **
*********************************/

#ifndef LOG_H
#define LOG_H

#include <boost/utility.hpp> // for boost::noncopyable
#include <string>

enum verbosity_t {
	V_QUIET,   //! Display fatals.
	V_NORMAL,  //! Display fatals and errors.
	V_VERBOSE  //! Display fatals, errors and info.
};

class Log : boost::noncopyable
{
public:
	/**
	 * Initialize the clock for log timestamps.
	 */
	static bool init();

	/**
	 * Set the logging verbosity. Some log messages may be suppressed depending 
	 * on this setting.
	 */
	static void setVerbosity(verbosity_t mode);

	/**
	 * Log an info message to the console if verbosity is "V_VERBOSE".
	 */
	static void info(std::string domain, std::string msg);

	/**
	 * Log an error message to the console if verbosity is "V_VERBOSE" or
	 * "V_NORMAL".
	 */
	static void err(std::string domain, std::string msg);

	/**
	 * Log a fatal error message to the console.
	 */
	static void fatal(std::string domain, std::string msg);

	/**
	 * Used by main() to report the verbosity setting on engine startup.
	 */
	static void reportVerbosityOnStartup();

private:
	Log();
};

/**
 * Exports Log hooks to the python interpreter.
 */
void exportLog();

#endif

