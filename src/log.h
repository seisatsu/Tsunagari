/******************************
** Tsunagari Tile Engine     **
** log.h                     **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>

//! Logging Mode Enum
/*!
	This enum defines the logging modes.
*/
enum message_mode_t {
	MM_SILENT, // Only error messages are sent to the console.
	MM_DEVELOPER, // Error messages and development messages are sent to the
	              // console.
	MM_DEBUG // All messages are sent to the console.
};

//! Log Class
/*!
	This is a singleton that handles error messages.
*/
class Log
{
public:
	//! Set message mode. (SILENT, DEVELOPER, or DEBUG)
	static void setMode(message_mode_t mode);

	//! Send a message to the console.
	/*!
		@param domain the name of the message's origin (traditionally, the
			name of the function we're in)
		@param message the message to be sent
	*/
	static void err(std::string domain, std::string message);
	static void dev(std::string domain, std::string message);
	static void dbg(std::string domain, std::string message);

private:
	static Log* instance(); // Access to our singleton instance.
	static std::string& rtrim(std::string& str);

	Log() {}; // Hide our constructor.

	static Log* pInstance; // Our class instance.
	message_mode_t mode; // Message mode that has been set.
};

#endif

