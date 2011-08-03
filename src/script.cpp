/******************************
** Tsunagari Tile Engine     **
** script.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include "log.h"
#include "script.h"

Script::Script()
{
	L = lua_open();
	luaL_openlibs(L);
}

void Script::addFn(const char* name, lua_CFunction fn)
{
	lua_register(L, name, fn);
}

void Script::addInt(const char* name, lua_Integer i)
{
	lua_pushinteger(L, i);
	lua_setglobal(L, name);
}

void Script::addData(const char* name, void* data)
{
	lua_pushlightuserdata(L, data);
	lua_setglobal(L, name);
}

void Script::run(const char* fn)
{
	if (luaL_loadfile(L, fn)) {
		Log::err("Script::run", std::string("Couldn't load file: ") +
		               lua_tostring(L, -1));
		return;
	}
	lua_call(L, 0, 0);
}

Script::~Script()
{
	lua_close(L);
}

