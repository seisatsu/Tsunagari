if drinking == False:
	drinking = True
	color_up = True
	overlay_alpha = 0
	snd = Sound.play("sounds/splash.oga") # splash sound
	snd.speed = 1.0 + randfloat(-0.1, 0.1)

