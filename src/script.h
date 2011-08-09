/******************************
** Tsunagari Tile Engine     **
** script.h                  **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>

#include <lua.hpp>

enum ObjType {
	ENTITY
};

class Entity;
class Resourcer;

//! Compile and execute Lua code at runtime.
/*!
	A Script represents a self-contained Lua state. It might have
	variables, functions, statements, and expressions, and can be executed.
	Each Script lives in a separate memory space and cannot directly
	access or call other Scripts.

	On their own, Scripts cannot change anything in a game. Instead, you
	must bind C functions that they can call. These C functions can access
	the C++ objects in the game. You might bind a function that makes a
	player jump, or that returns the distance to the nearest town.  You can
	also bind variables that a script can access.

	Scripts use Lua 5.1
*/
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


	//! Bind a C function to a global variable in Lua.
	void bindGlobalFn(const std::string& name, lua_CFunction fn);

	//! Bind a C function to a table index. In Lua: "table.index = fn"
	void bindObjFn(const std::string& table, const std::string& index, lua_CFunction fn);

	//! Set an integer to a table index.
	void bindObjInt(const std::string& table, const std::string& name, lua_Integer i);

	//! Set a global integer variable in Lua.
	void bindInt(const std::string& name, lua_Integer i);


	//! Create a global table with the specified name which represents a
	//C++ object.
	void bindObj(const std::string& bindTo, ObjType type, void* obj,
	             const luaL_Reg* funcs);

	//! Get the Entity object from a table in the Lua stack at a specific
	//position.
	Entity* getEntity(int pos);


	//! Compile and run a script, keeping all existing bindings intact.
	void run(Resourcer* rc, const std::string& fn);

private:
	//! Did we create our state, or are we borrowing it from another Script
	//object?
	bool ownState;

	//! Everything necessary to compile and run a Lua script.
	lua_State* L; // TODO: change to shared_ptr ?
};

#endif

