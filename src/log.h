/******************************
** Tsunagari Tile Engine     **
** log.h                     **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>

enum message_mode_t {
	MM_SILENT, // Only error messages are sent to the console.
	MM_DEVELOPER, // Error messages and development messages are sent to the
	              // console.
	MM_DEBUG // All messages are sent to the console.
};

// This is a singleton to handle console messages.
class Log
{
public:
	/**
	 * Set message mode. (SILENT, DEVELOPER, or DEBUG)
	 */
	static void setMode(message_mode_t mode);

	/**
	 * Send a message to the console.
	 *
	 * @param messageDomain the name of the message's origin
	 * @param message the message to be sent
	 */
	static void err(std::string domain, std::string message);
	static void dev(std::string domain, std::string message);
	static void dbg(std::string domain, std::string message);

private:
	/**
	 * Access to our singleton instance.
	 */
	static Log* instance();

	/**
	 * Hide our constructor.
	 */
	Log() {};

	static Log* pInstance; // Our class instance.
	message_mode_t mode; // Message mode that has been set.
};

#endif

