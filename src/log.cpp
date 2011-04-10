/******************************
** Tsunagari Tile Engine     **
** log.cpp                   **
** Copyright 2011 OmegaSDG   **
******************************/

#include "log.h"

// Singleton Enforcement
Log* Log::pInstance = NULL;

// Singleton Enforcement
Log* Log::instance()
{
	if (!pInstance)
		pInstance = new Log;

	return pInstance;
}

// Specify mode setting.
void Log::setMode(message_mode_t mode)
{
	Log* mh = instance();
	mh->mode = mode;
}

// Give output to the "write" function if it is allowed to be sent in the current
// mode.
void Log::err(std::string domain, std::string message)
{
	std::cerr << "Err: " << domain << ": " << message << std::endl;
}

void Log::dev(std::string domain, std::string message)
{
	Log* l = instance();
	if (l->mode == MM_DEBUG || l->mode == MM_DEVELOPER)
		std::cerr << "Dev: " << domain << ": " << message << std::endl;
}

void Log::dbg(std::string domain, std::string message)
{
	Log* l = instance();
	if (l->mode == MM_DEBUG)
		std::cerr << "Dbg: " << domain << ": " << message << std::endl;
}

