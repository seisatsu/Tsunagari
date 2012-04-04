def play_snd(filename):
    snd = resourcer.get_sound(filename)
    if snd: # snd will be None if game is run without audio
        snd.play()

play_snd("sounds/door.oga") # closing sound

