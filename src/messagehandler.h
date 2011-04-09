/******************************
** Tsunagari Tile Engine     **
** messagehandler.h          **
** Copyright 2011 OmegaSDG   **
******************************/

#include <iostream>
#include <string>

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

enum message_mode_t {
	SILENT, // Only error messages are sent to the console.
	VERBOSE, // Error messages and console messages are sent to the console.
	DEVELOPER // All messages are sent to the console.
};

enum message_type_t {
	MSG, // Console Message
	ERR, // Error Message
	DEV // Developer Mode Message
};

// This is a singleton to handle console messages.
class MessageHandler
{
public:
	// Singleton Access
	static MessageHandler* console();
	
	// Set message mode. (SILENT, VERBOSE, DEVELOPER)
	void setMode(message_mode_t mode);
	
	// Send a message to the console.
	//+ "type" is MSG, ERR, or DEV.
	//+ "messageDomain" is the name of the message's origin. This can be anything.
	//+ "message" is the message to be sent.
	void send(message_type_t type, std::string messageDomain, std::string message);

private:
	MessageHandler(){}; // Hide our constructor.
	static MessageHandler* m_pInstance; // Our class instance.
	message_mode_t mode; // Message mode that has been set.
	void write(message_type_t type, std::string messageDomain, std::string message); // Write to console.
};

#endif

