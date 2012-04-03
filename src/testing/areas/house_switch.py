def playSnd(filename):
    snd = resourcer.getSound(filename)
    if snd: # snd will be None if game is run without audio
        snd.play()

# Unlock the door!
if unlockedTheDoor == False:
    unlockedTheDoor = True
    tile = area.getTile(4, 0, 0.0) # closed door on north wall, property layer
    tile.door = tsunagari.Door("areas/secret_room.tmx", 4, 4, 0.0)
    tile.walkable = True
    tile = area.getTile(4, 0, -0.2) # closed door on north wall, graphics layer
    tile.type = area.getTileType(66) # change to open door
    area.requestRedraw()
    playSnd("sounds/door.oga") # unlocking sound

