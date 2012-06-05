# This script is called when the chest in grove02.tmx is activated by the
# player. The first time we are run, we open the chest. Further invocations
# feature an easter egg where we toggle the game's music. :)

def grove02_toggle_music():
	if Music.paused:
		Music.paused = False
		print 'Unpausing music!'
	else:
		Music.paused = True
		print 'Pausing music!'

# Open the chest!
def grove02_open_chest():
	global grove02_opened_chest
	if not grove02_opened_chest:
		grove02_opened_chest = True
		tile = Area.tile(5, 2, -0.05) # closed chest
		tile2 = tile.offset(0, -1) # above the closed chest
		tile.type = Area.tileset("areas/tiles/objects.png").at(1, 6) # change to open chest, button half
		tile2.type = Area.tileset("areas/tiles/objects.png").at(1, 5) # change to open chest, top half
		Area.redraw()
		Sound.play("sounds/door.oga") # unlocking sound
	else:
		grove02_toggle_music()

