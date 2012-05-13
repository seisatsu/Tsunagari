class stdlib_ai_dumb:
	def __init__(self, descriptor, phase, x, y, layer, tickspeed, movechance):
		# Prepare timer.
		self.__timer = new_timer()
		self.__timer.running = True

		# Save some variables.
		self.__tickspeed = tickspeed
		self.__movechance = movechance

		# Initialize and insert the entity.
		self.e_inst = tsunagari.Entity()
		self.e_inst.init(descriptor)
		self.e_inst.add_on_update_listener(self.tick)
		self.e_inst.phase = phase
		self.e_inst.area = Area
		self.e_inst.set_coords(x, y, layer)

		Area.add_entity(self.e_inst)

	def tick(self): # Called 60 times per second.
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
			if self.e_inst.tile.offset(-1, 0).flag.nowalk:
				self.do_move()
			else:
				self.e_inst.move(-1, 0)

		elif move_choice == 1:
			if self.e_inst.tile.offset(1, 0).flag.nowalk:
				self.do_move()
			else:
				self.e_inst.move(1, 0)

		elif move_choice == 2:
			if self.e_inst.tile.offset(0, -1).flag.nowalk:
				self.do_move()
			else:
				self.e_inst.move(0, -1)

		elif move_choice == 3:
			if self.e_inst.tile.offset(0, 1).flag.nowalk:
				self.do_move()
			else:
				self.e_inst.move(0, 1)

