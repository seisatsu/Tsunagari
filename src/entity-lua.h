/******************************
** Tsunagari Tile Engine     **
** entity-lua.h              **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef ENTITY_LUA_H
#define ENTITY_LUA_H

#include <lua.hpp>

#include "entity.h"
#include "script.h"

//! Bind an Entity to a Lua table.
/*
	Responsible for creating Lua bindings around an Entity object.
	Creates a Lua table named with "bindTo" and saves the Entity to a
	semi-private member. Adds Lua methods that access the Entity and
	binds some variables from the Entity to the Lua table.
*/
void bindEntity(Script& script, Entity* entity, const std::string& bindTo);

#endif

