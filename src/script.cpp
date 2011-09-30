/******************************
** Tsunagari Tile Engine     **
** script.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include "common.h"
#include "log.h"
#include "resourcer.h"
#include "script.h"
#include "script-sound.h"

struct CppObj {
	ObjType type;
	void* data;
};

Script::Script(Resourcer* rc)
	: ownState(true), L(lua_open())
{
	luaL_openlibs(L);
	bindSound(this, rc);
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

void Script::bindGlobalFn(const std::string& name, lua_CFunction fn)
{
	lua_register(L, name.c_str(), fn);
}

void Script::bindObjFn(const std::string& table, const std::string& index,
                       lua_CFunction fn)
{
	// Get table.
	lua_getglobal(L, table.c_str());

	// table.name = fn
	lua_pushcfunction(L, fn);
	lua_setfield(L, -2, index.c_str());

	// Done with table.
	lua_remove(L, -1);
}

void Script::bindObjInt(const std::string& table, const std::string& index,
                        lua_Integer i)
{
	// Get table.
	lua_getglobal(L, table.c_str());

	// table.name = fn
	lua_pushinteger(L, i);
	lua_setfield(L, -2, index.c_str());

	// Done with table.
	lua_remove(L, -1);
}

void Script::bindInt(const std::string& name, lua_Integer i)
{
	lua_pushinteger(L, i);
	lua_setglobal(L, name.c_str());
}

void Script::bindObj(const std::string& bindTo, ObjType type, void* obj,
                     const luaL_Reg* funcs)
{
	// Create table, store the object.
	lua_createtable(L, 0, 3);
	newCppObj(type, obj);
	lua_setfield(L, -2, "object");

	// Add methods to table.
	for (; funcs->name; funcs++) {
		lua_pushcclosure(L, funcs->func, 0);
		lua_setfield(L, -2, funcs->name);
	}

	// Save table.
	lua_setglobal(L, bindTo.c_str());
}

std::string Script::getString(int loc)
{
	if (!lua_isstring(L, loc))
		return "";
	size_t len;
	const char* s = lua_tolstring(L, loc, &len);
	return std::string(s, len);
}

void* Script::getObj(int table, ObjType type)
{
	// Get table.object
	if (!lua_istable(L, table))
		return NULL;
	lua_getfield(L, table, "object");

	// Check if table.object has the type we asked for
	if (!lua_isuserdata(L, -1))
		return NULL;
	CppObj* obj = (CppObj*)lua_touserdata(L, -1);
	return obj->type == type ? obj->data : NULL;
}

void Script::run(Resourcer* rc, const std::string& fn)
{
	if (!rc->getLuaScript(fn.c_str(), L)) // TODO: error logged
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

void Script::newCppObj(ObjType type, void* data)
{
	CppObj* o = (CppObj*)lua_newuserdata(L, sizeof(CppObj));
	o->data = data;
	o->type = type;
}

