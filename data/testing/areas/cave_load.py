if cave_fall_stage == 0:
	Player.frozen = True
	Player.phase = "up"
	overlay_alpha = 255
	Area.color_overlay(0, 0, 0, overlay_alpha)

