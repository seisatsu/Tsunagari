def grove01_load():
	global grove01_ai_wizard
	grove01_ai_wizard = stdlib_ai_wander("entities/wizard/wizard.xml", "down", 6, 3, 0.0, 1.0, 4)

def grove01_update():
	global grove01_drinking, grove01_duration, grove01_max_alpha
	if grove01_drinking == True:
		progress = grove01_well_timer.count / grove01_duration
		if progress < 0.5:
			overlay_alpha = int(2 * grove01_max_alpha * progress)
		elif progress < 1.0:
			overlay_alpha = int(2 * grove01_max_alpha * (1 - progress))
		else:
			overlay_alpha = 0
			grove01_drinking = False
		Area.color_overlay(255, 255, 255, overlay_alpha) # white overlay

def grove01_well():
	global grove01_drinking, grove01_well_timer
	if grove01_drinking == False:
		grove01_drinking = True

		grove01_well_timer = new_timer()
		grove01_well_timer.running = True

		sound_splash()

