import areas.sounds

opened_chest = False

def toggle_music():
#	if Music.paused:
#		Music.paused = False
#		log('Unpausing music!')
#	else:
#		Music.paused = True
#		log('Pausing music!')
	pass

def open_chest():
	# This function is called when the chest in grove02.tmx is activated by
	# the player. The first time we are run, we open the chest. Further
	# invocations feature an easter egg where we toggle the game's music. :)
	
	global opened_chest
	if not opened_chest:
		opened_chest = True
		tile = Area.tile(5, 2, -0.05) # closed chest
		tile2 = tile.offset(0, -1) # above the closed chest
		tile.type = Area.tileset("areas/tiles/objects.png").at(1, 6) # change to open chest, button half
		tile2.type = Area.tileset("areas/tiles/objects.png").at(1, 5) # change to open chest, top half
		Area.redraw()
		areas.sounds.sound_door()
	else:
		toggle_music()

