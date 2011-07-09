/******************************
** Tsunagari Tile Engine     **
** cmd.cpp                   **
** Copyright 2011 OmegaSDG   **
******************************/

#include <iomanip>
#include <iostream>

#include "cmd.h"

CommandLineOptions::CommandLineOptions(int argc, char** argv) :
	argc(argc), argv(argv)
{
}

CommandLineOptions::~CommandLineOptions()
{
	unsigned int opt;
	
	for (opt = 0; opt < OptionsList.size(); opt++)
		delete OptionsList[opt];
}

void CommandLineOptions::insert(std::string shortopt, std::string longopt, 
	std::string argument, std::string description)
{
	Option* opt = new Option;
	opt->shortopt = shortopt;
	opt->longopt = longopt;
	opt->argument = argument;
	opt->description = description;
	opt->present = false;
	opt->value = "";
	OptionsList.push_back(opt);
}

bool CommandLineOptions::parse()
{
	int arg;
	unsigned int opt;
	bool found;
	
	for (arg = 1; arg < argc; arg++) {
		found = false;
		for (opt = 0; opt < OptionsList.size(); opt++) {
			if (!OptionsList[opt]->shortopt.compare(argv[arg]) || 
			    !OptionsList[opt]->longopt.compare(argv[arg])) {
				found = true; /* We found the option. */
				OptionsList[opt]->present = true;
				if (!OptionsList[opt]->argument.empty()) {
					if (arg + 1 < argc) {
						OptionsList[opt]->value = argv[arg+1];
						arg += 1;
					}
					else
						return false;
				}
				break;
			}
		}
		if (!found) /* We didn't find the option. */
			return false;
	}
	return true;
}

bool CommandLineOptions::check(std::string longopt)
{
	unsigned int search;
	
	for (search = 0; search < OptionsList.size(); search++) {
		if (!OptionsList[search]->longopt.compare(longopt))
			return OptionsList[search]->present;
	}
	
	return false;
}

std::string CommandLineOptions::get(std::string longopt)
{
	unsigned int search;
	
	for (search = 0; search < OptionsList.size(); search++) {
		if (!OptionsList[search]->longopt.compare(longopt))
			return OptionsList[search]->value;
	}
	
	return NULL;
}

void CommandLineOptions::usage()
{
	//TODO: Formatting.
	unsigned int opt;
	
	std::cerr << "Usage: " << argv[0];
	
	for (opt = 0; opt < OptionsList.size(); opt++) {
		std::cerr << " [";
		if (!OptionsList[opt]->shortopt.empty())
			std::cerr << OptionsList[opt]->shortopt << "|";
		std::cerr << OptionsList[opt]->longopt;
		if (!OptionsList[opt]->argument.empty())
			std::cerr << " " << OptionsList[opt]->argument << 
			    "]";
		else
			std::cerr << "]";
	}
	
	std::cerr << std::endl << std::endl;
	
	for (opt = 0; opt < OptionsList.size(); opt++) {
		if (!OptionsList[opt]->shortopt.empty())
			std::cerr << OptionsList[opt]->shortopt << ",";
		std::cerr << OptionsList[opt]->longopt << " ";
		if (!OptionsList[opt]->argument.empty())
			std::cerr << OptionsList[opt]->argument << " ";
		std::cerr << OptionsList[opt]->description << std::endl;
	}
	
	std::cerr << std::endl;
}
