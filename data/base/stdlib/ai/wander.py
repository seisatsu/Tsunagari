class stdlib_ai_wander:
	def __init__(self, descriptor, phase, x, y, layer, tickspeed, movechance):
		# Prepare timer.
		self.__timer = new_timer()
		self.__timer.running = True

		# Save some variables.
		self.__tickspeed = tickspeed
		self.__movechance = movechance

		# Initialize and insert the entity.
		self.e_inst = Area.new_npc(descriptor, x, y, layer, phase)
		self.e_inst.on_tick = self.tick
		self.e_inst.on_turn = self.turn

	def tick(self): # Called for tick().
		if not self.e_inst.moving:
			if self.__timer.count >= self.__tickspeed:
				self.__timer.reset()

				# Decide whether or not to move.
				move_choice = randint(0, self.__movechance)
				if move_choice == 0:
					self.do_move()

	def turn(self): # Called for turn().
		move_choice = randint(0, self.__movechance)
		if move_choice == 0:
			self.do_move()

	def do_move(self): # Move the entity.
		move_choice = randint(0, 3)

		if move_choice == 0:
			dx, dy = -1, 0
		elif move_choice == 1:
			dx, dy = 1, 0
		elif move_choice == 2:
			dx, dy = 0, -1
		elif move_choice == 3:
			dx, dy = 0, 1

		e = self.e_inst
		x, y, z = e.move_dest(e.tile, dx, dy)
		can = e.can_move(x, y, z)
		if can:
			log("ai.wander: move: %d %d %.1f valid" % (x, y, z))
		else:
			log("ai.wander: move: %d %d %.1f invalid" % (x, y, z))
		e.move(dx, dy)

