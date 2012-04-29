def wizard_ai():
    if not Entity.moving:
        wizard_ai_calc_move()

def wizard_ai_calc_move():
    Entity.move(0, 1)

    #print "Python: Wizard AI: moving! [%d %d %f]" % entity.coords

