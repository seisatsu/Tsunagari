def wizard_ai():
    if not entity.moving:
        wizard_ai_calc_move()

def wizard_ai_calc_move():
    entity.move(0, 1)

    #print "Python: Wizard AI: moving! [%d %d %f]" % entity.coords

