/******************************
** Tsunagari Tile Engine     **
** messagehandler.cpp        **
** Copyright 2011 OmegaSDG   **
******************************/

#include "messagehandler.h"

// Singleton Enforcement
MessageHandler* MessageHandler::m_pInstance = NULL;

// Singleton Enforcement
MessageHandler* MessageHandler::console() {
	if (!m_pInstance)
		m_pInstance = new MessageHandler;
	
	return m_pInstance;
}

// Specify mode setting.
void MessageHandler::setMode(message_mode_t mode) {
	this->mode = mode;
}

// Give output to the "write" function if it is allowed to be sent in the current mode.
void MessageHandler::send(message_type_t type, std::string messageDomain, std::string message) {
	if (mode == SILENT) {
		if (type == ERR)
			write(type, messageDomain, message);
	}
	
	if (mode == VERBOSE) {
		if (type == ERR || type == MSG)
			 write(type, messageDomain, message);
	}
	
	if (mode == DEVELOPER)
		write(type, messageDomain, message);
}

// Construct and send the console output.
void MessageHandler::write(message_type_t type, std::string messageDomain, std::string message) {
	switch (type) {
		case MSG:
			std::cout << "Notify: " << messageDomain << ": " << message << "\n";
			std::cout.flush();
			break;
		case ERR:
			std::cerr << "Error: " << messageDomain << ": " << message << "\n";
			std::cerr.flush();
			break;
		case DEV:
			std::cerr << "Dev: " << messageDomain << ": " << message << "\n";
			std::cerr.flush();
			break;
	}
}

