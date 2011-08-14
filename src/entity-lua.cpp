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

static int lua_Entity_setSpeed(lua_State* L)
{
	Script script(L);
	if (lua_gettop(L) != 2)
		return luaL_error(L, "setSpeed needs 2 arg");
	Entity* entity = (Entity*)script.getObj(1, ENTITY);
	if (!entity)
		return luaL_error(L,
			 "setSpeed: arg 1 should be an Entity");
	if (!lua_isnumber(L, 2)) {
		return luaL_error(L,
			"setSpeed: arg 2 should be a positive non-zero number");
	}
	double speed = lua_tonumber(L, 2);
	if (speed <= 0) {
		return luaL_error(L,
			"setSpeed: arg 2 should be a positive non-zero number");
	}
	entity->setSpeed(speed);
	return 0;
}

static const luaL_Reg funcs[] = {
	{ "gotoRandomTile", lua_Entity_gotoRandomTile },
	{ "setSpeed", lua_Entity_setSpeed },
	{ NULL, NULL }
};

void bindEntity(Script& script, Entity* entity, const std::string& bindTo)
{
	const icoord_t tile = entity->getCoordsByTile();
	script.bindObj(bindTo, ENTITY, (void*)entity, funcs);
	script.bindObjInt(bindTo, "x", tile.x);
	script.bindObjInt(bindTo, "y", tile.y);
}

