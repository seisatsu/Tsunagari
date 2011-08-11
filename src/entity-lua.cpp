/******************************
** Tsunagari Tile Engine     **
** entity-lua.cpp            **
** Copyright 2011 OmegaSDG   **
******************************/

#include "entity-lua.h"

static int lua_Entity_gotoRandomTile(lua_State* L)
{
	Script script(L);
	if (lua_gettop(L) != 1)
		return luaL_error(L, "gotoRandomTile needs 1 arg");
	Entity* entity = (Entity*)script.getObj(1, ENTITY);
	if (!entity)
		return luaL_error(L,
			 "gotoRandomTile: arg 1 should be an Entity");
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

