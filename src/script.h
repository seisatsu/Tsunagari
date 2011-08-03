/******************************
** Tsunagari Tile Engine     **
** script.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>

#include <lua.hpp>

class Script
{
public:
	Script();
	~Script();

	void addFn(const char* name, lua_CFunction fn);
	void addInt(const char* name, lua_Integer i);
	void addData(const char* name, void* data);

	void run(const char* fn);

private:
	lua_State* L;
};

#endif

