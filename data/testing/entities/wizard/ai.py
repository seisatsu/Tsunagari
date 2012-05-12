def wizard_ai():
	if not Entity.moving:
		if wizard_timer.count >= 1:
			wizard_timer.reset()
			wizard_ai_calc_move()

def wizard_ai_calc_move():
		wizard_choice = randint(0, 3)
		if wizard_choice == 1:
			wizard_ai_do_move()

def wizard_ai_do_move():
	wizard_choice = randint(0, 3)
	if wizard_choice == 0:
		if Entity.tile.offset(-1, 0).flag.nowalk:
			wizard_ai_do_move()
		else:
			Entity.move(-1, 0)
	elif wizard_choice == 1:
		if Entity.tile.offset(1, 0).flag.nowalk:
			wizard_ai_do_move()
		else:
			Entity.move(1, 0)
	elif wizard_choice == 2:
		if Entity.tile.offset(0, -1).flag.nowalk:
			wizard_ai_do_move()
		else:
			Entity.move(0, -1)
	elif wizard_choice == 3:
		if Entity.tile.offset(0, 1).flag.nowalk:
			wizard_ai_do_move()
		else:
			Entity.move(0, 1)

	print "Python: Wizard AI: moving! [%d %d %.1f]" % Entity.coords

wizard_ai()
