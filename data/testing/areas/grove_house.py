# Unlock the door!
def house_open_door():
	global house_opened_door
	if house_opened_door == False:
		house_opened_door = True

		tile = Area.tile(4, 0, 0.0) # closed exit on north wall, property layer
		tile.exit = new_exit("areas/secret_room.tmx", 4, 5, 0.0)
		tile.flag.nowalk = False

		tile = Area.tile(4, 0, -0.2) # closed exit on north wall, graphics layer
		tile.type = Area.tileset("areas/tiles/indoors.png").at(2, 9) # change to open exit

		Area.redraw()
		Sound.play("sounds/door.oga") # unlocking sound

