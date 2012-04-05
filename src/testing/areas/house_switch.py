# Unlock the door!
if unlocked_the_door == False:
    unlocked_the_door = True

    tile = area.tiles(4, 0, 0.0) # closed exit on north wall, property layer
    tile.exit = tsunagari.Exit("areas/secret_room.tmx", 4, 4, 0.0)
    tile.walkable = True

    tile = area.tiles(4, 0, -0.2) # closed exit on north wall, graphics layer
    tile.type = area.get_tile_type(66) # change to open exit

    area.request_redraw()
    Sound.play("sounds/door.oga") # unlocking sound

