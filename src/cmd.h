/******************************
** Tsunagari Tile Engine     **
** cmd.h                     **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef CMD_H
#define CMD_H

#include <string>
#include <vector>

//! This class handles Tsunagari's command line input.
class CommandLineOptions
{
public:
	//! Initialize the parser with argc and argv.
	CommandLineOptions(int argc, char** argv);
	~CommandLineOptions();

	//! Add an option to the parser.
	/*!
		@param shortopt Full short option, ex. "-c", empty string if no 
		short option.
		@param longopt Full long option, ex. "--config", required.
		@param argument A short description of the argument expected to 
		the option, an empty string if no argument.
		@param description A description of the argument, required.
	*/
	void insert(std::string shortopt, std::string longopt, 
		std::string argument, std::string description);

	//! Process the command line.
	/*!
		Run after all of your inserts.
		@return true if succeeded, false if failed.
	*/
	bool parse();

	//! Check if option was used.
	/*!
		@return true if option was used, false otherwise.
	*/
	bool check(std::string longopt);

	//! Get the string value of an option's argument.
	/*!
		@return empty string if failed.
	*/
	std::string get(std::string longopt);

	//! Print a pretty usage/help message.
	/*!
		automatically formatted from the list of inserted options.
	*/
	void usage();

private:
	void usageSize(int* optmaxlen, int* argmaxlen);
	void usagePrintShort();
	void usagePrintLong(int optmaxlen, int argmaxlen);

	int argc;
	char** argv;

	struct Option {
		std::string shortopt;
		std::string longopt;
		std::string argument;
		std::string description;
		bool present;
		std::string value;
	};

	std::vector<Option*> OptionsList;
};

#endif

