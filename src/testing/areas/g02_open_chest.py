def playSnd(filename):
    snd = resourcer.getSound(filename)
    if snd: # snd will be None if game is run without audio
        snd.play()

# Open the chest!
if openedTheChest == False:
    openedTheChest = True
    tile = area.getTile(5, 2, -0.05) # closed chest
    tile2 = area.getTile(5, 1, -0.05) # closed chest, above
    tile.type = area.getTileType(367) # change to open chest
    tile2.type = area.getTileType(360) # change to open chest, above
    area.requestRedraw()
    playSnd("sounds/door.oga") # unlocking sound

