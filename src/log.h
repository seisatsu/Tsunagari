/*********************************
** Tsunagari Tile Engine        **
** log.h                        **
** Copyright 2011-2012 OmegaSDG **
*********************************/

// "OmegaSDG" is defined as Michael D. Reiley and Paul Merrill.

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

#ifndef LOG_H
#define LOG_H

#include <boost/utility.hpp> // for boost::noncopyable
#include <string>

enum verbosity_t {
	V_QUIET = 1,   //! Display fatals.
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

