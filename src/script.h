/******************************
** Tsunagari Tile Engine     **
** script.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>

#include <lua.hpp>

class Entity;

class Script
{
public:
	//! Create a new Lua state. The state is destroyed with the death of
	//this object.
	Script();

	//! If we already have a Lua state, wrap around it. The state is not
	//destroyed with the death of this object.
	Script(lua_State* L);

	//! Destroy the Lua state if we own it.
	~Script();

	void bindFn(const char* name, lua_CFunction fn);
	void bindInt(const char* name, lua_Integer i);

	void bindEntity(const char* name, Entity* entity);
	Entity* getEntity(int pos);

	void run(const char* fn);

private:
	bool ownState;
	lua_State* L; // TODO: change to shared_ptr ?
};

#endif

