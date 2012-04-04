def play_snd(filename):
    snd = resourcer.get_sound(filename)
    if snd: # snd will be None if game is run without audio
        snd.play()

# Unlock the door!
if unlocked_the_door == False:
    unlocked_the_door = True
    tile = area.get_tile(4, 0, 0.0) # closed door on north wall, property layer
    tile.door = tsunagari.Door("areas/secret_room.tmx", 4, 4, 0.0)
    tile.walkable = True
    tile = area.get_tile(4, 0, -0.2) # closed door on north wall, graphics layer
    tile.type = area.get_tile_type(66) # change to open door
    area.request_redraw()
    play_snd("sounds/door.oga") # unlocking sound

