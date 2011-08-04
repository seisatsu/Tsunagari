/******************************
** Tsunagari Tile Engine     **
** entity-lua.cpp            **
** Copyright 2011 OmegaSDG   **
******************************/

#include "entity.h"
#include "entity-lua.h"
#include "script.h"

int lua_Entity_gotoRandomTile(lua_State* L)
{
	Script script(L);

	int n = lua_gettop(L);
	if (n != 1) {
		Log::err("gotoRandomTile", "gotoRandomTile needs 1 argument");
		return 0;
	}

	Entity* entity = script.getEntity(1);
	if (!entity)
		return luaL_error(L, "gotoRandomTile: argument 1 should be an "
				"Entity");
	entity->gotoRandomTile();
	return 0;
}

