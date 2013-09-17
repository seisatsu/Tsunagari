def sound_armor():
	snd = Sound.play("sounds/metal_clank.oga") # armor sound
	snd.speed = 1.0 + randfloat(-0.3, 0.3)

def sound_book():
	snd = Sound.play("sounds/book.oga") # book sound
	snd.speed = 1.0 + randfloat(-0.3, 0.3)

def sound_door():
	snd = Sound.play("sounds/door.oga") # door opening sound
	snd.speed = 1.0 + randfloat(-0.3, 0.3)

def sound_ouch():
	snd = Sound.play("sounds/ouch.oga") # ouch sound
	snd.speed = 1.0 + randfloat(-0.1, 0.1)

def sound_splash():
	snd = Sound.play("sounds/splash.oga")
	snd.speed = 1.0 + randfloat(-0.1, 0.1)

