/******************************
** Tsunagari Tile Engine     **
** log.cpp                   **
** Copyright 2011 OmegaSDG   **
******************************/

#include "config.h"
#include "log.h"

//! Singleton enforcement.
Log* Log::instance()
{
	static Log* inst = new Log;
	return inst;
}

Log::Log()
{
	mode = MESSAGE_MODE;
}

//! Specify mode setting.
void Log::setMode(message_mode_t mode)
{
	Log* l = instance();
	l->mode = mode;
}

/*
 * Give output to the "write" function if it is allowed to be sent in
 * the current mode.
 */
void Log::err(std::string domain, std::string message)
{
	std::cerr << "Error [" << domain << "] - " << rtrim(message)
	          << std::endl;
}

void Log::dev(std::string domain, std::string message)
{
	Log* l = instance();
	if (l->mode == MM_DEBUG || l->mode == MM_DEVELOPER)
		std::cerr << "Devel [" << domain << "] - " << rtrim(message)
		          << std::endl;
}

void Log::dbg(std::string domain, std::string message)
{
	Log* l = instance();
	if (l->mode == MM_DEBUG)
		std::cerr << "Debug [" << domain << "] - " << rtrim(message)
		          << std::endl;
}

void Log::blank()
{
	std::cerr << std::endl;
}

std::string& Log::rtrim(std::string& str)
{
	if (str[str.length()-1] == '\n')
		str[str.length()-1] = '\0';
	return str;
}

