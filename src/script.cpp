/******************************
** Tsunagari Tile Engine     **
** script.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include "common.h"
#include "log.h"
#include "resourcer.h"
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

void Script::bindGlobalFn(const char* name, lua_CFunction fn)
{
	lua_register(L, name, fn);
}

void Script::bindObjFn(const char* table, const char* index, lua_CFunction fn)
{
	// Get table.
	lua_getglobal(L, table);

	// table.name = fn
	lua_pushstring(L, index);
	lua_pushcfunction(L, fn);
	lua_settable(L, -3);

	// Done with table.
	lua_remove(L, -1);
}

void Script::bindInt(const char* name, lua_Integer i)
{
	lua_pushinteger(L, i);
	lua_setglobal(L, name);
}

void Script::bindEntity(const char* name, Entity* entity)
{
	// Create table to hold our object and its functions/variables.
	lua_createtable(L, 0, 3);

	lua_pushstring(L, "object");

	// Create type-aware wrapper around Entity.
	CppObj* obj = (CppObj*)lua_newuserdata(L, sizeof(CppObj));
	obj->type = ENTITY;
	obj->entity = entity;

	// table.object = entity
	lua_settable(L, -3);

	// Bind table to Lua.
	lua_setglobal(L, name);
}

Entity* Script::getEntity(int pos)
{
	// Get table.object
	if (!lua_istable(L, pos))
		return NULL;
	lua_pushstring(L, "object");
	lua_gettable(L, pos);

	// Check if table.object is an Entity
	if (!lua_isuserdata(L, -1))
		return NULL;
	CppObj* obj = (CppObj*)lua_touserdata(L, -1);
	if (obj->type != ENTITY)
		return NULL;
	return obj->entity;
}

void Script::run(Resourcer* rc, const char* fn)
{
	if (!rc->getLuaScript(fn, L)) // error logged
		return;

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

