/******************************
** Tsunagari Tile Engine     **
** script-sound.cpp          **
** Copyright 2011 OmegaSDG   **
******************************/

#include <Gosu/Audio.hpp>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "resourcer.h"
#include "script-sound.h"

// TODO support looping, volume, speed, and pan
static int lua_Sound_play(lua_State* L)
{
	Script script(L);
	if (lua_gettop(L) != 2)
		return luaL_error(L, "play needs 2 args");
	Resourcer* rc = (Resourcer*)script.getObj(1, RESOURCER);
	if (!rc)
		return luaL_error(L,
			 "play: arg 1 should be a Sound");
	std::string fn = script.getString(2);
	if (fn.empty())
		return luaL_error(L,
			"play: arg 2 should be a non-empty string");
	SampleRef s = rc->getSample(fn);
	if (s)
		s->play(1, 1, 0);
	else
		return luaL_error(L, "play: sound file not found");
	return 0;
}

static const luaL_Reg funcs[] = {
	{ "play", lua_Sound_play },
	{ NULL, NULL }
};

void bindSound(Script& script, Resourcer* rc)
{
	script.bindObj("Sound", RESOURCER, rc, funcs);
}

