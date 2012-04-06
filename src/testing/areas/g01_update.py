if drinking == True:
	if color_up == True:
		overlay_alpha += 6
		if overlay_alpha > 192:
			color_up = False
		Area.color_overlay(255, 255, 255, overlay_alpha)
	elif color_up == False:
		overlay_alpha -= 6
		if overlay_alpha < 6:
			drinking = False
		Area.color_overlay(255, 255, 255, overlay_alpha)
