# This script is called when the chest in grove02.tmx is activated by the
# player. The first time we are run, we open the chest. Further invocations
# feature an easter egg where we toggle the game's music. :)

def toggle_music():
    if Music.paused:
        Music.paused = False
        print 'Unpausing music!'
    else:
        Music.paused = True
        print 'Pausing music!'

# Open the chest!
if not opened_the_chest:
    opened_the_chest = True
    tile = Area.tiles(5, 2, -0.05) # closed chest
    tile2 = tile.offset(0, -1) # above the closed chest
    tile.type = Area.get_tile_type(367) # change to open chest, button half
    tile2.type = Area.get_tile_type(360) # change to open chest, top half
    Area.request_redraw()
    Sound.play("sounds/door.oga") # unlocking sound
else:
    toggle_music()

