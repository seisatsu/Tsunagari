def playSnd(filename):
    snd = resourcer.getSample(filename)
    if snd: # snd will be None if game is run without audio
        snd.play()

def createDoor(area, x, y, z):
    door = tsunagari.Door()
    door.area = area
    door.tile = tsunagari.icoord(x, y, z)
    return door

def getTile(x, y, z):
    coord = tsunagari.icoord(x, y, area.depthIndex(z))
    tile = area.getTile(coord)
    return tile

# Open the chest!
if openedTheChest == False:
    openedTheChest = True
    tile = getTile(5, 2, -0.05) # closed chest
    tile2 = getTile(5, 1, -0.05) # closed chest, above
    tile.type = area.getTileType(367) # change to open chest
    tile2.type = area.getTileType(360) # change to open chest, above
    area.requestRedraw()
    playSnd("sounds/door.oga") # unlocking sound

