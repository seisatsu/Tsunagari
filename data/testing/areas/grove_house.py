import areas.sounds

opened_door = False

def open_door():
	# Unlock the door!
	global opened_door
	if opened_door == False:
		opened_door = True

		tile = Area.tile(4, 0, 0.0) # closed exit on north wall, property layer
		tile.exit = new_exit("areas/secret_room.tmx", 4, 5, 0.0)
		tile.flag.nowalk = False

		tile = Area.tile(4, 0, -0.2) # closed exit on north wall, graphics layer
		tile.type = Area.tileset("areas/tiles/indoors.png").at(2, 9) # change to open exit

		Area.redraw()
		areas.sounds.sound_door()

