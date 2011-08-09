/******************************
** Tsunagari Tile Engine     **
** entity-lua.cpp            **
** Copyright 2011 OmegaSDG   **
******************************/

#include "entity-lua.h"

static int lua_Entity_gotoRandomTile(lua_State* L)
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

static const luaL_Reg funcs[] = {
	{ "gotoRandomTile", lua_Entity_gotoRandomTile },
	{ NULL, NULL }
};

void bindEntity(Script& script, Entity* entity, const std::string& bindTo)
{
	const coord_t tile = entity->getCoordsByTile();
	script.bindObj(bindTo, ENTITY, (void*)entity, funcs);
	script.bindObjInt(bindTo, "x", tile.x);
	script.bindObjInt(bindTo, "y", tile.y);
}

