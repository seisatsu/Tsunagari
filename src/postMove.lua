#!/usr/bin/lua
--
-- This script is executed every time an entity finishes walking onto a tile.
--
-- We are called from within Entity::postMove() in entity.cpp. There, we are
-- passed some game data. We pass three variables to this script from the Entity
-- object: x, y, and entity, which represents the Entity itself.
--
-- We also have a C function we can call, gotoUpperLeft, which takes an Entity
-- object as a parameter.
--

print("Lua says: Entity is at " .. x .. ", " .. y)

-- The red portal teleports us! *gasp* Excitement!
if x == 11 and y == 4 then
	gotoUpperLeft(entity)
	-- The x,y variables in this script aren't updated yet, but they will
	-- be next time we're called.
end

