/******************************
** Tsunagari Tile Engine     **
** descriptor.h - Descriptor **
** Copyright 2011 OmegaSDG   **
******************************/

// An abstraction wrapper around LibYAML parser functionality.

#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <string>
#include <stdio.h>
#include <yaml.h>
#include "common.h"

class Descriptor
{
public:
	Descriptor(const std::string fn);
	~Descriptor();
	bool init();
private:
	FILE* fh;
	const char* yamlfile;
	yaml_parser_t yamlparser;
};

#endif

