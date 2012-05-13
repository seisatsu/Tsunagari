if cave_fall_stage == 0:
	cave_fall_stage = 1
	cave_fall_snd = Sound.play("sounds/rockfall.oga")

if cave_fall_stage == 1:
	if not cave_fall_snd.playing:
		cave_fall_stage = 2
		cave_fall_gfx_timer = new_timer()
		cave_fall_gfx_timer.running = True

if cave_fall_stage == 2:
	if cave_fall_gfx_timer.count > 0.02:
		cave_fall_gfx_timer.reset()
		if overlay_alpha > 0:
			overlay_alpha -= 5
			Area.color_overlay(0, 0, 0, overlay_alpha)
		else:
			cave_fall_stage = 3
			Entity.frozen = False

