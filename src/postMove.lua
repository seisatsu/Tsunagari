#!/usr/bin/lua
--
-- This script is executed every time an Entity finishes walking onto a tile.
--
-- We are called from within Entity::postMove() in entity.cpp. There, we are
-- passed some game data. We pass three variables to this script from the
-- Entity object: x, y, and entity, which represents the Entity itself.
--
-- We also have a C function we can call, gotoRandomTile, which takes an Entity
-- object as a parameter.
--

-- Returns true if we are standing on the specified X,Y location.
function on(X, Y)
	return x == X and y == Y
end

-- Returns true if we are standing on any of the specified locations.
function onAny(locations)
	for _,loc in ipairs(locations) do
		if on(loc.x, loc.y) then
			return true
		end
	end
	return false
end

local redPortalLocations = {
	{x = 11, y = 4},
	{x = 4, y = 11}
}

-- The red portals teleport us! *gasp* Excitement!
if onAny(redPortalLocations) then
	print("Teleported!")
	entity:gotoRandomTile()
end

