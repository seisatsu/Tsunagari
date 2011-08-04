/******************************
** Tsunagari Tile Engine     **
** script.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include "common.h"
#include "log.h"
#include "script.h"

enum ObjType {
	ENTITY
};

struct CppObj {
	ObjType type;
	union {
		Entity* entity;
	};
};

Script::Script()
	: ownState(true), L(lua_open())
{
	luaL_openlibs(L);
}

Script::Script(lua_State* L)
	:ownState(false), L(L)
{
}

Script::~Script()
{
	if (ownState)
		lua_close(L);
}

void Script::bindFn(const char* name, lua_CFunction fn)
{
	lua_register(L, name, fn);
}

void Script::bindInt(const char* name, lua_Integer i)
{
	lua_pushinteger(L, i);
	lua_setglobal(L, name);
}

void Script::bindEntity(const char* name, Entity* entity)
{
	CppObj* obj = (CppObj*)lua_newuserdata(L, sizeof(CppObj));
	obj->type = ENTITY;
	obj->entity = entity;
	lua_setglobal(L, name);
}

Entity* Script::getEntity(int pos)
{
	if (!lua_isuserdata(L, pos))
		return NULL;
	CppObj* obj = (CppObj*)lua_touserdata(L, pos);
	if (obj->type != ENTITY)
		return NULL;
	return obj->entity;
}

void Script::run(const char* fn)
{
	if (luaL_loadfile(L, fn)) {
		Log::err("Script::run", std::string("couldn't load file: ") +
		               lua_tostring(L, -1));
		return;
	}

	// TODO: make fourth parameter to lua_call an error handler so we can
	// gather stack trace through Lua state... we can't do it after the
	// call
	switch (lua_pcall(L, 0, 0, 0)) {
		case LUA_ERRRUN:
			Log::err("Script::run", lua_tostring(L, -1));
			break;
		case LUA_ERRMEM:
			// Should we even bother with this?
			Log::err("Script::run", "Lua: out of memory");
			break;
		case LUA_ERRERR:
			Log::err("Script::run", "error in Lua error facility"
					"... what did you do!?");
			break;
	}
}

