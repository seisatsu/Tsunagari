import random

snd = Sound.play("sounds/metal_clank.oga") # armor sound
snd.speed = 1.0 + (float(random.randint(-3, 3)) / 10)

