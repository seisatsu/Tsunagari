import areas.sounds
#import stdlib.ai.wander

ai_wizard = None
drinking = False
duration = 1.0
max_alpha = 192
well_timer = None

def load():
	#ai_wizard = stdlib_ai_wander("entities/wizard/wizard.xml", "down", 6, 3, 0.0, 1.0, 4)

	cloud = Area.new_overlay("entities/cloud/cloud.xml", 11, 2, 10.0, "down")
	cloud.move(-400, 0)

def tick():
	global drinking, duration, max_alpha
	if drinking == True:
		progress = well_timer.count / duration
		if progress < 0.5:
			overlay_alpha = int(2 * max_alpha * progress)
		elif progress < 1.0:
			overlay_alpha = int(2 * max_alpha * (1 - progress))
		else:
			overlay_alpha = 0
			drinking = False
		Area.color_overlay(255, 255, 255, overlay_alpha) # white overlay

def well():
	global drinking, well_timer
	if drinking == False:
		drinking = True

		well_timer = new_timer()
		well_timer.running = True

		areas.sounds.sound_splash()

