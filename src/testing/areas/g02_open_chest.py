def play_snd(filename):
    snd = resourcer.get_sound(filename)
    if snd: # snd will be None if game is run without audio
        snd.play()

# Open the chest!
if opened_the_chest == False:
    opened_the_chest = True
    tile = area.get_tile(5, 2, -0.05) # closed chest
    tile2 = area.get_tile(5, 1, -0.05) # closed chest, above
    tile.type = area.get_tile_type(367) # change to open chest
    tile2.type = area.get_tile_type(360) # change to open chest, above
    area.request_redraw()
    play_snd("sounds/door.oga") # unlocking sound

