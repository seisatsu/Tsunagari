class stdlib_ai_wander:
	def __init__(self, descriptor, phase, x, y, layer, tickspeed, movechance):
		# Prepare timer.
		self.__timer = new_timer()
		self.__timer.running = True

		# Save some variables.
		self.__tickspeed = tickspeed
		self.__movechance = movechance

		# Initialize and insert the entity.
		self.e_inst = Area.new_entity(descriptor, x, y, layer, phase)
		self.e_inst.on_update = self.tick

	def tick(self): # Called on update.
		if not self.e_inst.moving:
			if self.__timer.count >= self.__tickspeed:
				self.__timer.reset()

				# Decide whether or not to move.
				move_choice = randint(0, self.__movechance)
				if move_choice == 0:
					self.do_move()

	def do_move(self): # Move the entity.
		move_choice = randint(0, 3)

		if move_choice == 0:
			self.e_inst.move(-1, 0)

		elif move_choice == 1:
			self.e_inst.move(1, 0)

		elif move_choice == 2:
			self.e_inst.move(0, -1)

		elif move_choice == 3:
			self.e_inst.move(0, 1)

