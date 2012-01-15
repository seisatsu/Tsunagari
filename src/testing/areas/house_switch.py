def playSnd(filename):
    snd = resourcer.getSample(filename)
    if snd: # snd will be None if game is run without audio
        snd.play()

def createDoor(area, x, y, z):
    door = tsunagari.Door()
    door.area = area
    door.tile = tsunagari.vicoord(x, y, z)
    return door

# Unlock the door!
if unlockedTheDoor == False:
    unlockedTheDoor = True
    tile = area.getTile(4, 0, 0.0) # closed door on north wall
    tile.walkable = True
    tile.type = area.getTileType(55) # change to open door
    tile.door = createDoor("areas/secret_room.tmx", 4, 4, 0.0)
    area.requestRedraw()
    playSnd("sounds/door.oga") # unlocking sound

