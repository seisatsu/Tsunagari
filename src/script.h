/******************************
** Tsunagari Tile Engine     **
** script.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>

#include <lua.hpp>

#include "entity.h"

class Script
{
public:
	Script();
	~Script();

	bool init(const std::string& filename);

	void registerCppFn(const std::string& name, Entity* obj, void* fn);
//	void* getObj(const std::string& name);
	bool run();

private:
	lua_State* L;
	std::string name;
	void* obj;
};

#endif

