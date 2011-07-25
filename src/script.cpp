/******************************
** Tsunagari Tile Engine     **
** script.cpp                **
** Copyright 2011 OmegaSDG   **
******************************/

#include <string.h>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "log.h"
#include "script.h"

static void stackDump (lua_State *L)
{
	int top = lua_gettop(L);
	for (int i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		switch (t) {

		case LUA_TSTRING:  /* strings */
			printf("‘%s’", lua_tostring(L, i));
			break;

		case LUA_TBOOLEAN:  /* booleans */
			printf(lua_toboolean(L, i) ? "true" : "false");
			break;

		case LUA_TNUMBER:  /* numbers */
			printf("%g", lua_tonumber(L, i));
			break;

		default:  /* other values */
			printf("%s", lua_typename(L, t));
			break;

		}
		printf("  ");  /* put a separator */
	}
	printf("\n");  /* end the listing */
}

Script::Script()
{
	L = lua_open();
	luaL_openlibs(L);
}

bool Script::init(const std::string& name)
{
	if (luaL_loadfile(L, name.c_str())) {
		Log::err(name, std::string("Couldn't load file: ") +
		               lua_tostring(L, -1));
		return false;
	}
	return true;
}

void Script::registerCppFn(const std::string& name, Entity* obj, void* fn)
{
	boost::function<void()> bound = boost::bind(&Entity::moveByPixel, obj, coord(1, 0, 0));
	int sz = sizeof(bound);

	void* ud = lua_newuserdata(L, sz);
	memcpy(ud, &bound, sz);
}

/*
void Script::registerObj(const std::string& name, void* obj)
{
	const char* key = name.c_str();

	// registry[name] = obj
	this->name = name;
	this->obj = obj;

	lua_register(L, key, callCppFn);
}
*/

/* Lua Registry
	lua_pushlightuserdata(L, this->name);
	lua_pushlightuserdata(L, this->obj);
	lua_settable(L, LUA_REGISTRYINDEX);
*/

/* Create Lua table
	lua_newtable(L);
	lua_setglobal(L, key);

        lua_pushnumber(L, i);   // Push the table index
        lua_pushnumber(L, i*2); // Push the cell value
        lua_rawset(L, -3);      // Stores the pair in the table
*/

//#include <stdio.h>
//#include <stdlib.h>

// void Script::createTable()
// {
//     int result, i;
//     double sum;

//     /*
//      * Ok, now here we go: We pass data to the lua script on the stack.
//      * That is, we first have to prepare Lua's virtual stack the way we
//      * want the script to receive it, then ask Lua to run it.
//      */
//     lua_newtable(L);    /* We will pass a table */

//     /*
//      * To put values into the table, we first push the index, then the
//      * value, and then call lua_rawset() with the index of the table in the
//      * stack. Let's see why it's -3: In Lua, the value -1 always refers to
//      * the top of the stack. When you create the table with lua_newtable(),
//      * the table gets pushed into the top of the stack. When you push the
//      * index and then the cell value, the stack looks like:
//      *
//      * <- [stack bottom] -- table, index, value [top]
//      *
//      * So the -1 will refer to the cell value, thus -3 is used to refer to
//      * the table itself. Note that lua_rawset() pops the two last elements
//      * of the stack, so that after it has been called, the table is at the
//      * top of the stack.
//      */
//     for (i = 1; i <= 5; i++) {
//         lua_pushnumber(L, i);   /* Push the table index */
//         lua_pushnumber(L, i*2); /* Push the cell value */
//         lua_rawset(L, -3);      /* Stores the pair in the table */
//     }

//     /* By what name is the script going to reference our table? */
//     lua_setglobal(L, "foo");

//     /* Ask Lua to run our little script */
//     result = lua_pcall(L, 0, LUA_MULTRET, 0);
//     if (result) {
//         fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
//         exit(1);
//     }

//     /* Get the returned value at the top of the stack (index -1) */
//     sum = lua_tonumber(L, -1);

//     printf("Script returned: %.0f\n", sum);

//     lua_pop(L, 1);  /* Take the returned value out of the stack */
// }

/*
void* Script::getObj(const std::string&)
{
	// variable with an unique address
	static const char Key = 'k';

	// retrieve a number
	lua_pushlightuserdata(L, (void *)&Key);  // push address
	lua_gettable(L, LUA_REGISTRYINDEX);  // retrieve value
	void* obj = lua_touserdata(L, -1);  // convert to number
	return obj;
}
*/

bool Script::run()
{
	lua_call(L, 0, 0);
	return true;
}

Script::~Script()
{
	lua_close(L);
}

