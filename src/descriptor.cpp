/******************************
** Tsunagari Tile Engine     **
** descriptor.cpp            **
** Copyright 2011 OmegaSDG   **
******************************/

#include "descriptor.h"

Descriptor::Descriptor(const std::string fn) {
	yamlfile = strtochar(fn);
}

Descriptor::~Descriptor() {
	yaml_parser_delete(&yamlparser);
	fclose(fh);
	delete yamlfile;
}

bool Descriptor::init() {
	fh = fopen(yamlfile, "r");
	if (!yaml_parser_initialize(&yamlparser) or fh == NULL)
		return false;
	else {
		yaml_parser_set_input_file(&yamlparser, fh);
		return true;
	}
}

