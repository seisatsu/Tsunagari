fall_gfx_timer = None
fall_snd = None
fall_stage = 0
overlay_alpha = 0

def load():
	global overlay_alpha
	if fall_stage == 0:
		Player.frozen = True
		Player.phase = "up"
		overlay_alpha = 255
		Area.color_overlay(0, 0, 0, overlay_alpha)

def tick():
	global fall_gfx_timer, fall_snd, fall_stage, overlay_alpha
	if fall_stage == 0:
		fall_stage = 1
		fall_snd = 	snd = Sound.play("sounds/rockfall.oga")

	if fall_stage == 1:
		if not fall_snd.playing:
			fall_stage = 2
			fall_gfx_timer = new_timer()
			fall_gfx_timer.running = True

	if fall_stage == 2:
		if fall_gfx_timer.count > 0.02:
			fall_gfx_timer.reset()
			if overlay_alpha > 0:
				overlay_alpha -= 5
				Area.color_overlay(0, 0, 0, overlay_alpha)
			else:
				fall_stage = 3
				Player.frozen = False

