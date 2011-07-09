/******************************
** Tsunagari Tile Engine     **
** cmd.h                     **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef CMD_H
#define CMD_H

#include <string>
#include <vector>

class CommandLineOptions
{
public:
	CommandLineOptions(int argc, char** argv);
	~CommandLineOptions();
	
	/* Add an option to the parser. */
	void insert(std::string shortopt, std::string longopt, 
		std::string argument, std::string description);
	
	/* Process the command line. Returns true if succeeded, false if 
	   failed.*/
	bool parse();
	
	/* Check if option was used. */
	bool check(std::string longopt);
	
	/* Get the string value of an option's argument. */
	std::string get(std::string longopt);
	
	/* Print a pretty usage/help message. */
	void usage();
	
private:
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

