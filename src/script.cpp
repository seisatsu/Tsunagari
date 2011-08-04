/******************************
** Tsunagari Tile Engine     **
** script.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

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
//	if (!lua_isuserdata(L, pos)) {
//		Log::err("gotoRandomTile", "gotoRandomTile's first argument "
//				"needs to be a userdata");
//		return 0;
//	}

	CppObj* obj = (CppObj*)lua_touserdata(L, pos);
//	assert obj->type == ENTITY

	return obj->entity;
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

