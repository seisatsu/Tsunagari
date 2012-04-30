if drinking == False:
    drinking = True

    well_timer = new_timer()
    well_timer.running = True

    snd = Sound.play("sounds/splash.oga")
    snd.speed = 1.0 + randfloat(-0.1, 0.1)

