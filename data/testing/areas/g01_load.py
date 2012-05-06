import entities.wizard.ai

drinking = False

wizard_timer = new_timer()
wizard_timer.running = True

e = tsunagari.Entity()
e.init("entities/wizard/wizard.xml")
e.add_on_update_listener(wizard_ai)
e.animation = "down"
e.area = Area
e.set_coords(6, 3, 0.0)

Area.add_entity(e)

