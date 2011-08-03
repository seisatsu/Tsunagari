/******************************
** Tsunagari Tile Engine     **
** entity-lua.cpp            **
** Copyright 2011 OmegaSDG   **
******************************/

#include "entity.h"
#include "entity-lua.h"

int lua_Entity_gotoRandomTile(lua_State* L)
{
	int n = lua_gettop(L);
	if (n != 1) {
		Log::err("gotoRandomTile", "gotoRandomTile needs 1 argument");
		return 0;
	}
	if (!lua_isuserdata(L, 1)) {
		Log::err("gotoRandomTile", "gotoRandomTile's first argument "
				"needs to be a userdata");
		return 0;
	}
	// assert obj.type == Entity*

	Entity* obj = (Entity*)lua_touserdata(L, 1);
	obj->gotoRandomTile();
	return 0;
}

